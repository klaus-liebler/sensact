using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class MasterApplication : SensactNodeApplication
    {
        public MasterApplication():base("MASTER")
        {
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("MASTER");
            }
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            return string.Empty;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return null;
        }
    }
}
