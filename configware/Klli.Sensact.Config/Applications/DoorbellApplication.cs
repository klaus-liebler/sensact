using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class DoorbellApplication : ActorApplication
    {
        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            HashSet<CommandType> ret = new HashSet<CommandType>()
            {
                CommandType.SET_SIGNAL,
            };
            return ret;
        }

        public override void OnSET_SIGNALCommand(ushort signal)
        {
            base.OnSET_SIGNALCommand(signal);
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// DOORBELL {0}" + Environment.NewLine, ApplicationId);
            //cBell DOORBELL("DOORBELL", eApplicationID::DOORBELL, &MODEL::volumeSchedule);
            sb.AppendFormat("sensact::cBell {0}(\"{0}\", eApplicationID::{0}, &MODEL::volumeSchedule);"+Environment.NewLine + Environment.NewLine, ApplicationId);
            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("DEVCE"+REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}
