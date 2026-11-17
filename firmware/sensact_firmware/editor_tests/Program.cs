using System.Xml.Linq;
using Microsoft.Extensions.FileProviders;
using SensactEditor.Services;

// Arbeitet auf einer Kopie von model/ -- die echten Dateien bleiben unberuehrt
var source = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "model"));
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
Check(blind.Fields.Select(f => f.Name).SequenceEqual(["id", "number", "description", "relay1", "relay2", "relayMode", "upTimeSec", "downTimeSec"]), "Feldreihenfolge blind");
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
