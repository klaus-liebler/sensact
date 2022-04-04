using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public abstract class PushButtonApplication : SensorApplication
    {
        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>
            {
                EventType.PRESSED,
                EventType.PRESSED_SHORT_AND_HOLD,
                EventType.RELEASED,
                EventType.RELEASED_SHORT,
                EventType.RELEASED_LONG,
                EventType.DOUBLE_CLICKED,
            };
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("PUSHB");
            }
        }
    }
    
    public abstract class PushButtonSingleApplication : PushButtonApplication
    {
        public ushort InputRessource;

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputRessource.ToString()))
            {
                return "InputRessource of Button "+ApplicationId;
            }

            usedInputPins.Add(InputRessource.ToString());
            return null;
        }
    }

    public abstract class PushButtonDualApplication : PushButtonApplication
    {
        public ushort InputRessource1;
        public ushort InputRessource2;
        

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputRessource1.ToString()) || usedInputPins.Contains(InputRessource2.ToString())
            {
                return "InputRessource of Button "+ApplicationId;
            }

            usedInputPins.Add(InputRessource1.ToString());
            usedInputPins.Add(InputRessource2.ToString());
            return null;
        }
    }


    public class PushButtonSingle2OnOffApplication :PushButtonSingleApplication{
        
        public string TargetApplicationId; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonSingle2OnOff {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonSingle2OnOff {0}(eApplicationID::{0}, {1}, eApplicationID::{2});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputRessource, 
                TargetApplicationId
                );

            return sb.ToString();
        }
    }

    public class PushButtonSingle2PwmSingleApplication :PushButtonSingleApplication{
        
        public string targetApplicationId; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonSingle2PwmSingle {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonSingle2PwmSingle {0}(eApplicationID::{0}, {1}, eApplicationID::{2});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputRessource, 
                targetApplicationId
                );

            return sb.ToString();
        }
    }

    public class PushButtonDual2BlindApplication :PushButtonDualApplication{
        
        public string targetApplicationId; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonDual2Blind {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonDual2Blind {0}(eApplicationID::{0}, {1}, {2}, eApplicationID::{3});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputRessource1,
                InputRessource2,
                targetApplicationId
                );

            return sb.ToString();
        }
    }

    public class PushButtonDual2PWMApplication :PushButtonDualApplication{
        
        public string targetApplicationId; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonDual2PWM {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonDual2PWM {0}(eApplicationID::{0}, {1}, {2}, eApplicationID::{3});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputRessource1,
                InputRessource2,
                targetApplicationId
                );

            return sb.ToString();
        }
    }
}
