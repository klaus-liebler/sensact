using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class PoweritemApplication : ActorApplication
    {
        public OutputPin OutputRessource;
        public long AutoOffInterval; 
        public List<Event> ToggleEvents;
        public List<Event> OffEvents;
        public List<Event> OnEvents;

        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            HashSet<CommandType> ret = new HashSet<CommandType>()
            {
                CommandType.ON,
                CommandType.TOGGLE,
                CommandType.TOGGLE_SPECIAL,
            };
            return ret;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// POWIT {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::cPoweritem {0}(\"{0}\", eApplicationID::{0}, ePoweredOutput::{1}, {2});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessource, AutoOffInterval);
            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("POWIT" + REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}
