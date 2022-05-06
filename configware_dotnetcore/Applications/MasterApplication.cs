using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class MasterApplication : SensactNodeApplication
    {
        public MasterApplication():base((ushort)Nodes.ApplicationId.MASTER, Nodes.ApplicationId.MASTER.ToString())
        {
        }

        internal override Regex AppNameRegex
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
