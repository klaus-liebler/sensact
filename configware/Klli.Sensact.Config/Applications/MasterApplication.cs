using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Klli.Sensact.Config.Applications
{
    public class MasterApplication : SensactApplication
    {
        public MasterApplication()
        {
            this.ApplicationId = "MASTER";
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
