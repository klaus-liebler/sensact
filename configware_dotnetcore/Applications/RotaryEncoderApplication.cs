using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class RotaryEncoder2PWMApplication : SensorApplication
    {
         public RotaryEncoder2PWMApplication(ushort ApplicationId, string ApplicationName, RotaryEncoder InputRotaryRessource, ushort targetApplication):base(ApplicationId, ApplicationName){
            this.InputRotaryRessource=InputRotaryRessource;
            this.TargetApplication=targetApplication;
        }
        public RotaryEncoder InputRotaryRessource{get;}
        public ushort TargetApplication{get;}


        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputRotaryRessource.ToString()))
            {
                return "InputPushRessource of "+ApplicationId;
            }
            usedInputPins.Add(InputRotaryRessource.ToString());
            return null;
        }


        public override HashSet<EventType> ICanSendTheseEvents()
        {
            HashSet<EventType> ret = new HashSet<EventType>()
            {
                EventType.PRESSED,
                EventType.RELEASED,
                EventType.RELEASED_SHORT,
                EventType.RELEASED_MEDIUM
            };
            return ret;
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            //Command op_a1[1] = {{eApplicationID::POWIT_EG_WOZ_L1, eCommandType::BACKWARD }};
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// RotaryEncoder2PWM {0}" + Environment.NewLine, ApplicationId);
            
            sb.AppendFormat("sensact::apps::cRotaryEncoder2PWM {0}(eApplicationID::{0}, eRotaryEncoder::{1}, {2});" + Environment.NewLine + Environment.NewLine, ApplicationId, InputRotaryRessource, this.TargetApplication);

            return sb.ToString();
        }

        internal override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("ROTAR");
            }
        }
    }
}
