// Zeigt den per GitInfoReader gelesenen Stand an -- eigenstaendig aufrufbar/testbar, obwohl das
// Ergebnis derzeit noch nirgends generiert wird (s. Kommentar in GitInfo.cs: sensact_firmware
// braucht das erst als Zutat fuer den spaeteren Config-/Runtimeconfig-Writer-Schritt). Bis dahin
// dient diese Phase als Verifikation der Lese-Logik selbst.
namespace Builder.Phases;

public static class GitStatus
{
	public static void Run()
	{
		var info = GitInfoReader.ReadGitInfo();
		Console.WriteLine("      Commit Hash: " + info.CommitHash);
		Console.WriteLine("            Branch: " + info.Branch);
		Console.WriteLine("               Tag: " + info.Tag);
		Console.WriteLine("       Commit Date: " + DateTimeOffset.FromUnixTimeSeconds(info.CommitDateEpoch).ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss"));
		Console.WriteLine("     Commit Author: " + info.CommitAuthor);
		Console.WriteLine("    Commit Message: " + info.CommitMessage);
		Console.WriteLine("         Is Dirty?: " + (info.IsDirty ? "yes" : "no"));
		Console.WriteLine("           Version: " + info.Version);
	}
}
