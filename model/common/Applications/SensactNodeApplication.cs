using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class SensactNodeApplication : SensactApplication
    {
        public SensactNodeApplication(ushort ApplicationId, string ApplicationName):base(ApplicationId, ApplicationName){

        }
        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("SNSCT");
            }
        }

        public override string GenerateInitializer(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AFL("// Sensact node management application {0}", ApplicationName);
            //sb.AppendFormat("//is already hardcoded");
            sb.AF2L("sensact::apps::cSensactNode {0}(eApplicationID::{0});", ApplicationName);
            return sb.ToString();
        }

        public override string GenerateHTMLUserInterface(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
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

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return string.Empty;
        }

    }
}
