using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class PlaceholderApplication : ActorApplication
    {
        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            return string.Empty;
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return null;
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex(".*");
            }
        }
    }
}
