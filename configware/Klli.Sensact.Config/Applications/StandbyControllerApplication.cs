using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class StandbyControllerApplication : ActorApplication
    {
        public OutputPin OutputRessource;
        public long WaittimeInMsec;

        [SensactCommandMethod]
        public override void OnHEARTBEATCommand()
        {
            base.OnHEARTBEATCommand();
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// STDBY {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::cStandbyController {0}(\"{0}\", eApplicationID::{0}, ePoweredOutput::{1}, {2});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessource, WaittimeInMsec);
            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("STDBY"+REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}
