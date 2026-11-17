namespace Klli.Sensact.Model.Common
{
    /// <summary>Name und Nummer einer App-ID (Eintrag von eApplicationID).</summary>
    public sealed record AppIdEntry(string Name, ushort Number)
    {
        public override string ToString() => Name;
    }

    /// <summary>
    /// Alle App-IDs eines Hausmodells (auch vergebene ohne App), aufsteigend nach Nummer. Quelle ist
    /// entweder das C#-Enum ApplicationId eines C#-Hausmodells oder das XML-Hausmodell
    /// (Stationen, Apps, &lt;reserved&gt;). MASTER (0) und NO_APPLICATION (65535) sind immer enthalten.
    /// </summary>
    public sealed class AppIdTable
    {
        public const string MASTER = "MASTER";
        public const string NO_APPLICATION = "NO_APPLICATION";

        public IReadOnlyList<AppIdEntry> Entries { get; }

        private AppIdTable(IEnumerable<AppIdEntry> entries)
        {
            Entries = entries.OrderBy(e => e.Number).ToList();
            var dupNumber = Entries.GroupBy(e => e.Number).FirstOrDefault(g => g.Count() > 1);
            if (dupNumber != null)
            {
                throw new ArgumentException($"App-Nummer {dupNumber.Key} mehrfach vergeben: {string.Join(", ", dupNumber)}");
            }
            var dupName = Entries.GroupBy(e => e.Name).FirstOrDefault(g => g.Count() > 1);
            if (dupName != null)
            {
                throw new ArgumentException($"App-Name {dupName.Key} mehrfach vergeben.");
            }
            if (!Entries.Any(e => e.Name == MASTER && e.Number == Model.MASTER_APPLICATION_ID))
            {
                throw new ArgumentException($"{MASTER} fehlt oder hat nicht die Nummer {Model.MASTER_APPLICATION_ID}.");
            }
            if (!Entries.Any(e => e.Name == NO_APPLICATION && e.Number == Model.NO_APPLICATION_ID))
            {
                throw new ArgumentException($"{NO_APPLICATION} fehlt oder hat nicht die Nummer {Model.NO_APPLICATION_ID}.");
            }
        }

        public AppIdEntry NoApp => Entries.Last();
        public AppIdEntry Master => Entries.First();

        public static AppIdTable FromEnum<T>() where T : struct, Enum =>
            new(Enum.GetValues<T>().Select(v => new AppIdEntry(v.ToString(), Convert.ToUInt16(v))));

        /// <summary>Aus dem XML-Hausmodell; MASTER und NO_APPLICATION werden ergaenzt.</summary>
        public static AppIdTable FromNamesWithoutSpecials(IEnumerable<AppIdEntry> entries) =>
            new(entries.Append(new AppIdEntry(MASTER, Model.MASTER_APPLICATION_ID)).Append(new AppIdEntry(NO_APPLICATION, Model.NO_APPLICATION_ID)));
    }
}
