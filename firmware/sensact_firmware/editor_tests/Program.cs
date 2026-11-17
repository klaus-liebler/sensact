using System.Xml.Linq;
using Microsoft.Extensions.FileProviders;
using SensactEditor.Services;

// Arbeitet auf einer Kopie von model/ -- die echten Dateien bleiben unberuehrt
// model/ suchen: vom Arbeitsverzeichnis bzw. Programmverzeichnis aufwaerts (Ausgabeverzeichnis kann abweichen)
static string? FindModelDir(string start)
{
	for (var d = new DirectoryInfo(start); d != null; d = d.Parent)
	{
		if (File.Exists(Path.Combine(d.FullName, "model", "catalog.xml"))) return Path.Combine(d.FullName, "model");
	}
	return null;
}
var source = FindModelDir(Environment.CurrentDirectory) ?? FindModelDir(AppContext.BaseDirectory)
	?? throw new DirectoryNotFoundException("model/catalog.xml nicht gefunden (aus sensact_firmware heraus starten).");
var dir = Path.Combine(Path.GetTempPath(), "sensact_editor_tests", Guid.NewGuid().ToString("N"));
foreach (var f in Directory.EnumerateFiles(source, "*", SearchOption.AllDirectories))
{
	var dest = Path.Combine(dir, Path.GetRelativePath(source, f));
	Directory.CreateDirectory(Path.GetDirectoryName(dest)!);
	File.Copy(f, dest);
}
var cfg = new ConfigurationBuilder().AddInMemoryCollection(new Dictionary<string, string?> { ["ModelDir"] = dir, ["House"] = "Testmodel" }).Build();
var svc = new HouseModelService(cfg, new Env());
int fails = 0;
void Check(bool ok, string what) { Console.WriteLine($"{(ok ? "OK  " : "FAIL")} {what}"); if (!ok) fails++; }

string path = Path.Combine(dir, "Testmodel.xml");
string original = File.ReadAllText(path);

// 1) unveraendert speichern = byte-gleich (Formatierung bleibt)
svc.SetAttribute(svc.AllApps.First(), "description", (string?)svc.AllApps.First().Attribute("description"));
svc.Save();
Check(File.ReadAllText(path) == original, "Speichern ohne inhaltliche Aenderung ist byte-gleich");

// 2) Schema-Typen
Check(svc.Schema.Types.Count == 18, $"18 App-Typen aus dem XSD (gefunden {svc.Schema.Types.Count})");
var blind = svc.Schema.Types["blind"];
Check(blind.Fields.Select(f => f.Name).SequenceEqual(["id", "number", "description", "relay1", "relay2", "relayMode", "upTime", "downTime"]), "Feldreihenfolge blind");
Check(blind.Fields.First(f => f.Name == "relayMode").Choices.Count == 6, "relayMode als Auswahl mit 6 Werten");
Check(svc.Schema.Types["toggleButton"].Fields.First(f => f.Name == "targets").Kind == FieldKind.AppRefList, "targets = App-Liste");
Check(svc.Schema.Types["toggleButton"].Fields.First(f => f.Name == "input").Direction == IoDirectionHint.Input, "input = Eingang");
Check(svc.Schema.Types["singlePwm"].Fields.First(f => f.Name == "standbyController").Kind == FieldKind.AppRef, "standbyController = App-Verweis");
Check(blind.Doc != null && blind.Doc.Contains("Rollladen"), $"Beschreibung aus XSD-Kommentar: {blind.Doc}");

// 3) Ungueltige Aenderung wird erkannt, Speichern verweigert
var hs = svc.Stations.First();
var btn = HouseModelService.AppsOf(hs).First(a => a.Name == "toggleButton");
string oldInput = (string)btn.Attribute("input")!;
svc.SetAttribute(btn, "input", "IN0.99");
Check(svc.Issues.Any(i => i.Message.Contains("IN0.99")), "Kanal 99 wird gemeldet: " + string.Join(" | ", svc.Issues.Select(i => i.Message)));
try { svc.Save(); Check(false, "Speichern mit Fehler verweigert"); } catch (InvalidOperationException) { Check(true, "Speichern mit Fehler verweigert"); }
svc.SetAttribute(btn, "input", oldInput);
Check(svc.Issues.Count == 0, "nach Korrektur fehlerfrei");

// 4) Kommando-Pruefung aus der Codegenerierung
svc.SetAttribute(btn, "targets", HouseModelService.Id(svc.AllApps.First(a => a.Name == "toggleButton" && a != btn)));
Check(svc.Issues.Any(i => i.Source == "Modell"), "Ziel ohne TOGGLE wird gemeldet: " + string.Join(" | ", svc.Issues.Select(i => i.Message)));
svc.Reload();
hs = svc.Stations.First();

// 5) App hinzufuegen: Nummer neu, Pflichtfelder fehlen -> Fehler; ausgefuellt -> ok, gespeichert
int next = svc.NextFreeNumber;
var app = svc.AddApp(hs, "onOff", "POWIT_X9_XX9_TEST", "Testlampe");
Check((int)app.Attribute("number")! == next, $"neue Nummer {next}");
Check(svc.Issues.Any(), "fehlender Ausgang wird gemeldet");
svc.SetAttribute(app, "output", "OUT0.15");
Check(svc.Issues.Count == 0, "mit Ausgang fehlerfrei: " + string.Join(" | ", svc.Issues.Select(i => i.Message)));
svc.Save();
var saved = XDocument.Load(path);
Check(saved.Descendants("onOff").Any(e => (string?)e.Attribute("id") == "POWIT_X9_XX9_TEST"), "neue App in der Datei");
string text = File.ReadAllText(path);
Check(text.Contains("\n    <onOff id=\"POWIT_X9_XX9_TEST\""), "neue App eingerueckt wie ihre Nachbarn");

// 6) Loeschen: Verweis verhindert, sonst -> reserved
var target = svc.AllApps.First(a => svc.ReferencesTo(HouseModelService.Id(a)).Count > 0);
try { svc.DeleteApp(target); Check(false, "Loeschen trotz Verweis verweigert"); } catch (InvalidOperationException e) { Check(true, "Loeschen trotz Verweis verweigert: " + e.Message); }
var created = svc.AllApps.First(a => HouseModelService.Id(a) == "POWIT_X9_XX9_TEST");
svc.DeleteApp(created);
svc.Save();
saved = XDocument.Load(path);
Check(saved.Root!.Elements("reserved").Any(r => (string?)r.Attribute("id") == "POWIT_X9_XX9_TEST" && (int)r.Attribute("number")! == next), "geloeschte App als reserved mit ihrer Nummer");
Check(svc.NextFreeNumber == next + 1, "Nummer wird nicht wiederverwendet");
try { svc.AddApp(hs, "onOff", "POWIT_X9_XX9_TEST", null); Check(false, "Name einer reservierten App nicht neu vergeben"); } catch (ArgumentException) { Check(true, "Name einer reservierten App nicht neu vergeben"); }

// 7) Einheiten: Dauer und Prozent
svc.Reload();
hs = svc.Stations.First();
var lamp = HouseModelService.AppsOf(hs).First(a => a.Name == "onOff");
Check(svc.Schema.Types["onOff"].Fields.First(f => f.Name == "autoOff").Kind == FieldKind.Duration, "autoOff = Dauer");
Check(svc.Schema.Types["singlePwm"].Fields.First(f => f.Name == "minimalBrightness").Kind == FieldKind.Percent, "minimalBrightness = Prozent");
svc.SetAttribute(lamp, "autoOff", "PT1H30M");
Check(svc.Issues.Count == 0, "PT1H30M gueltig");
Check(AppSchema.DescribeDuration("PT1H30M") == "= 1 h 30 min", "Anzeige 1 h 30 min: " + AppSchema.DescribeDuration("PT1H30M"));
svc.SetAttribute(lamp, "autoOff", "30 Minuten");
Check(svc.Issues.Any(i => i.Message.Contains("autoOff")), "Freitext statt ISO wird gemeldet");
svc.SetAttribute(lamp, "autoOff", "P60D");
Check(svc.Issues.Any(i => i.Message.Contains("autoOff")), "60 Tage (> u32 ms) wird gemeldet: " + string.Join(" | ", svc.Issues.Select(i => i.Message)));
svc.SetAttribute(lamp, "autoOff", "PT30M");
var pwm = svc.AllApps.First(a => a.Name == "singlePwm");
svc.SetAttribute(pwm, "minimalBrightness", "5");
Check(svc.Issues.Count == 0, "5 % gueltig");
svc.SetAttribute(pwm, "minimalBrightness", "120");
Check(svc.Issues.Any(), "120 % wird gemeldet");
svc.Reload();
Check(Klli.Sensact.Model.Xml.Units.PercentToU16("23.047") == 15104 && Klli.Sensact.Model.Xml.Units.U16ToPercent(512) == "0.781", "Prozent <-> u16 exakt");
var blindApp = svc.AllApps.First(a => a.Name == "blind");
svc.SetAttribute(blindApp, "upTime", "PT2.5S");
Check(svc.Issues.Any(i => i.Message.Contains("ganze Sekunden")), "Rollladen-Fahrzeit nur ganze Sekunden");
svc.Reload();

// 8) Umbenennen: Verweise werden mitgezogen, Nummer bleibt
svc.Reload();
var renamed = svc.AllApps.First(a => HouseModelService.Id(a) == "POWIT_X1_XX3_9");
int renamedNumber = (int)renamed.Attribute("number")!;
int changedRefs = svc.RenameApp(renamed, "POWIT_X1_XX3_LAMP");
Check(changedRefs == 2, $"2 Verweise angepasst ({changedRefs})");
Check((int)renamed.Attribute("number")! == renamedNumber, "Nummer bleibt beim Umbenennen");
Check(svc.Issues.Count == 0, "nach Umbenennen fehlerfrei: " + string.Join(" | ", svc.Issues.Select(i => i.Message)));
Check(svc.ReferencesTo("POWIT_X1_XX3_LAMP").Count == 2 && svc.ReferencesTo("POWIT_X1_XX3_9").Count == 0, "Verweise zeigen auf den neuen Namen");
try { svc.RenameApp(renamed, "SOUND_X2_XX2_0"); Check(false, "vergebener Name abgelehnt"); } catch (ArgumentException) { Check(true, "vergebener Name abgelehnt"); }
try { svc.RenameApp(svc.Stations.First(), "SNSCT_NEU"); Check(false, "Station nicht umbenennbar"); } catch (InvalidOperationException) { Check(true, "Station nicht umbenennbar"); }
svc.Reload();

Console.WriteLine(fails == 0 ? "ALLE TESTS OK" : $"{fails} FEHLGESCHLAGEN");
return fails;

sealed class Env : IWebHostEnvironment
{
	public string WebRootPath { get; set; } = "";
	public IFileProvider WebRootFileProvider { get; set; } = new NullFileProvider();
	public string ApplicationName { get; set; } = "test";
	public IFileProvider ContentRootFileProvider { get; set; } = new NullFileProvider();
	public string ContentRootPath { get; set; } = AppContext.BaseDirectory;
	public string EnvironmentName { get; set; } = "Test";
}
