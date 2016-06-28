using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class BlindApplication : ActorApplication
    {
        public OutputPin OutputRessourceUpOrPower;
        public OutputPin OutputRessourceDown;
        public RelayMode RelayMode;
        public List<Event> FullyCloseEvents;
        public List<Event> FullyOpenEvents;
        public long OpenCloseTimeInSeconds;

        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            HashSet<CommandType> ret = new HashSet<CommandType>()
            {
                CommandType.UP,
                CommandType.DOWN,
                CommandType.STOP
            };
            return ret;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(Model m)
        {
            if (FullyCloseEvents != null || FullyOpenEvents != null)
            {
                throw new NotImplementedException("Events of Blinds");
            }
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// Blind {0}"+Environment.NewLine, ApplicationId);
            sb.AppendFormat("cBlind {0}(\"{0}\", eApplicationID::{0}, ePoweredOutput::{1}, ePoweredOutput::{2}, eRelayMode::{3}, {4});"+Environment.NewLine+Environment.NewLine, ApplicationId, OutputRessourceUpOrPower, OutputRessourceDown, RelayMode, OpenCloseTimeInSeconds);
            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("BLIND"+REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}
