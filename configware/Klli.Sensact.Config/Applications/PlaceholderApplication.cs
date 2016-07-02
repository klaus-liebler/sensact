using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class PlaceholderApplication : ActorApplication
    {


        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            var foo= new HashSet<CommandType>();
            foreach(CommandType ct in Enum.GetValues(typeof(CommandType)))
            {
                foo.Add(ct);
            }
            return foo;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            return string.Empty;
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
