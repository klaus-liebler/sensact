// Portierung von key_value_file_helper.ts (npm-packages/@klaus-liebler/espidf-vite-secure-build-tools)
// -- reines Text-Rendering eines flachen "defines"-Dictionary in drei Zielformate. Vollstaendig
// generisch, keine STM32-/sensact-Spezifika (die stecken im Aufbau des Dictionary selbst, s.
// Phases/GenerateRuntimeConfig.cs).
using System.Text;
using System.Text.Encodings.Web;
using System.Text.Json;

namespace Builder;

public static class RuntimeConfigWriter
{
	// System.Text.Json escaped standardmaessig alle Nicht-ASCII-Zeichen (z.B. "Sattlerstraße" ->
	// "Sattlerstraße") -- JS' JSON.stringify tut das nicht. Relaxed Encoder fuer lesbarere,
	// zum TS-Original passendere Ausgabe (funktional aequivalent, nur weniger comment-unfreundliches Escaping).
	private static readonly JsonSerializerOptions Options = new() { Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping };

	// Entspricht TypeScripts JSON.stringify(v) fuer die hier vorkommenden Werttypen (string, Zahl,
	// bool, string[]) -- Anfuehrungszeichen/Escaping fuer Strings, Klammern fuer Arrays, sonst roh.
	private static string StringifyValue(object value) => JsonSerializer.Serialize(value, Options);

	public static void CreateCppConfigurationHeader(string outDir, IReadOnlyDictionary<string, object> defines)
	{
		Directory.CreateDirectory(outDir);

		var definesSb = new StringBuilder("#pragma once\n");
		foreach (var (k, v) in defines)
		{
			definesSb.Append($"#define __{k}__ {StringifyValue(v)}\n");
		}
		File.WriteAllText(Path.Combine(outDir, "runtimeconfig_defines.hh"), definesSb.ToString());

		var namespaceSb = new StringBuilder("#pragma once\nnamespace cfg{\n");
		foreach (var (k, v) in defines)
		{
			namespaceSb.Append($"\tconstexpr auto {k}{{{StringifyValue(v)}}};\n");
		}
		namespaceSb.Append("}//namespace\n");
		File.WriteAllText(Path.Combine(outDir, "runtimeconfig.hh"), namespaceSb.ToString());
	}

	public static void CreateCMakeJsonConfigFile(string outDir, IReadOnlyDictionary<string, object> defines)
	{
		Directory.CreateDirectory(outDir);
		File.WriteAllText(Path.Combine(outDir, "config.json"), JsonSerializer.Serialize(defines, Options));
	}

	public static void CreateTypeScriptRuntimeConfig(string outDir, IReadOnlyDictionary<string, object> defines)
	{
		Directory.CreateDirectory(outDir);
		var sb = new StringBuilder();
		foreach (var (k, v) in defines)
		{
			sb.Append($"export const {k}={StringifyValue(v)}\n");
		}
		File.WriteAllText(Path.Combine(outDir, "index.ts"), sb.ToString());
		// TODO(npm.CreateAndInstallNpmProjectLazily-Aequivalent): die Vorlage (npm.ts) legt hier
		// zusaetzlich ein eigenstaendiges npm-Paket (package.json + npm install) an, damit
		// web/ diesen generierten Ordner per Workspace-Referenz importieren kann. Fuer die reine
		// Text-Generierung nicht noetig -- wird nachgeholt, sobald diese Phase tatsaechlich in den
		// Web-Build eingehaengt wird.
	}
}
