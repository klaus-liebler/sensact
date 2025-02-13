using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class MasterApplication : SensactNodeApplication
    {
        public MasterApplication(ushort ApplicationId, string ApplicationName) : base(ApplicationId, ApplicationName)
        {
        }

        public override Regex AppNameRegex
        {
            get
            {
                return new Regex("MASTER");
            }
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new();
        }
    }
}
