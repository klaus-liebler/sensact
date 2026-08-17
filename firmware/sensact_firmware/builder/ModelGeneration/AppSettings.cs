namespace Klli.Sensact.Config
{
    public class AppSettings
    {
        public SourceCodeGeneratorOptions? SourceCodeGenerator { get; set; }
    }
    public class SourceCodeGeneratorOptions
    {
        public const string SectionName = "SourceCodeGenerator";

        public string? BasePath { get; set; }
        public int ANumber { get; set; }
    }
}

