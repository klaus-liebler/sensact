using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class SensactNodeApplication : SensactApplication
    {
        public SensactNodeApplication(ushort ApplicationId, string ApplicationName):base(ApplicationId, ApplicationName){

        }
        internal override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("SNSCT");
            }
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// Sensact node management application {0} " + Environment.NewLine, ApplicationId);
            //sb.AppendFormat("//is already hardcoded");
            sb.AppendFormat("sensact::apps::cSensactNode {0}(eApplicationID::{0});" + Environment.NewLine + Environment.NewLine, ApplicationId);
            return sb.ToString();
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType> { EventType.PONG, EventType.IAP_READY };
        }

        [SensactCommandMethod]
        public override void OnPINGCommand(uint payload)
        {

        }

        [SensactCommandMethod]
        public override void OnSTART_IAPCommand()
        {

        }

        [SensactCommandMethod]
        public override void OnSET_PWMCommand(uint channelBitmask, ushort value)
        {

        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return null;
        }

    }
}
