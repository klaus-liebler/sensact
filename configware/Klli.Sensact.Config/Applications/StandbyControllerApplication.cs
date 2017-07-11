using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class StandbyControllerApplication : ActorApplication
    {
        public ushort OutputRessource;
        public long WaittimeInMsec;

        [SensactCommandMethod]
        public override void OnHEARTBEATCommand(uint sender)
        {
            base.OnHEARTBEATCommand(sender);
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessource.ToString()))
            {
                return "OutputRessource";
            }

            usedOutputPins.Add(OutputRessource.ToString());
            return null;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// STDBY {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::cStandbyController {0}(eApplicationID::{0}, {1}, {2});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessource, WaittimeInMsec);
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
