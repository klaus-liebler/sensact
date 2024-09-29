using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class RotaryEncoder2PWMApplication : SensorApplication
    {
         public RotaryEncoder2PWMApplication(ushort ApplicationId, string ApplicationName, ushort InputRotaryRessource, ushort targetApplication):base(ApplicationId, ApplicationName){
            this.InputRotaryRessource=InputRotaryRessource;
            this.TargetApplication=targetApplication;
        }
        public ushort InputRotaryRessource{get;}
        public ushort TargetApplication{get;}


        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputRotaryRessource.ToString()))
            {
                return "InputPushRessource of "+ApplicationId;
            }
            usedInputPins.Add(InputRotaryRessource.ToString());
            return string.Empty;
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

        public override string GenerateInitializer(ModelContainerForCodeGenerator m)
        {
            //Command op_a1[1] = {{eApplicationID::POWIT_EG_WOZ_L1, eCommandType::BACKWARD }};
            StringBuilder sb = new StringBuilder();
            sb.AFL("// RotaryEncoder2PWM {0}", ApplicationId);
            
            sb.AF2L("sensact::apps::cRotaryEncoder2PWM {0}(eApplicationID::{0}, eRotaryEncoder::{1}, {2});", ApplicationName, InputRotaryRessource, m.GetNameFromId(TargetApplication));

            return sb.ToString();
        }

        public override string GenerateTypescriptUserInterface(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("ROTAR");
            }
        }
    }
}
