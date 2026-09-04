// Kleiner, von Hand gepflegter Katalog NODE_ID -> (BoardTypeName, BoardTypeVersion) -- backt
// IBuildContext.BoardTypeName/BoardTypeVersion auf Sensact-Seite. BoardTypeName entspricht dem
// Namespace-Namen der jeweiligen main/hal/hal_sensact*.hh, die main/hal/<NODE_ID>/ (aktuell
// hal.inc, nach der HAL-Init-Klassen-Migration board_init.hh) einbindet -- MUSS parallel zu
// Aenderungen dort gepflegt werden (kein automatischer Abgleich, Drift ist moeglich).
// BoardTypeVersion ist mangels echter Hardware-Revisions-Versionierung vorerst ueberall ein Platzhalter.
namespace Builder;

public static class NodeIdBoardTypeCatalog
{
	private static readonly IReadOnlyDictionary<string, (string BoardTypeName, string BoardTypeVersion)> Catalog =
		new Dictionary<string, (string, string)>(StringComparer.Ordinal)
		{
			["MONITORING"] = ("SensactHsNano3", "1.0.0"),
			["SNSCT_NODE_HS"] = ("SensactHsNano3", "1.0.0"),
			["SNSCT_L3_WORK_HS"] = ("SensactUpHs", "1.0.0"),
			["SNSCT_NODE_SIDEDOOR"] = ("SensactUpControl", "1.0.0"),
			["SNSCT_NODE_TERRASSE"] = ("SensactOutdoor", "1.0.0"),
			["SNSCT_NODE_UP"] = ("SensactUp3", "1.0.0"),
		};

	public static (string BoardTypeName, string BoardTypeVersion) Resolve(string nodeId)
	{
		if (Catalog.TryGetValue(nodeId, out var entry))
		{
			return entry;
		}

		throw new InvalidOperationException(
			$"Unbekannte NODE_ID \"{nodeId}\" in NodeIdBoardTypeCatalog.cs -- welche hal_sensact*.hh bindet " +
			$"main/hal/{nodeId}/board_init.hh ein? Dort einen Eintrag ergaenzen.");
	}
}
