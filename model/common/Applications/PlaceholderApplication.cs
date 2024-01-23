using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class PlaceholderApplication : ActorApplication
    {
        public PlaceholderApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription):base(ApplicationId, ApplicationName, ApplicationDescription){}
        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override string GenerateHTMLUserInterface(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return string.Empty;
        }

        public override Regex AppNameRegex
        {
            get
            {
                return new Regex(".*");
            }
        }
    }
}
