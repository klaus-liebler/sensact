using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public abstract class PushButtonApplication : SensorApplication
    {
        protected PushButtonApplication(string ApplicationId):base(ApplicationId){}
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
        protected PushButtonSingleApplication(string ApplicationID, ushort InputResource):base(ApplicationID){
            this.InputResource=InputResource;
        }
        public ushort InputResource;

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputResource.ToString()))
            {
                return "InputRessource of Button "+ApplicationId;
            }

            usedInputPins.Add(InputResource.ToString());
            return null;
        }
    }

    public abstract class PushButtonDualApplication : PushButtonApplication
    {
        protected PushButtonDualApplication(string ApplicationID, ushort InputResource1, ushort InputResource2):base(ApplicationID){
            this.InputResource1=InputResource1;
            this.InputResource2=InputResource2;
        }
        public ushort InputResource1;
        public ushort InputResource2;
        

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputResource1.ToString()) || usedInputPins.Contains(InputResource2.ToString()))
            {
                return "InputRessource of Button "+ApplicationId;
            }

            usedInputPins.Add(InputResource1.ToString());
            usedInputPins.Add(InputResource2.ToString());
            return null;
        }
    }


    public class PushButtonSingle2ToggleApplication :PushButtonSingleApplication{
        
        public PushButtonSingle2ToggleApplication(string applicationId, ushort InputResource, ICollection<string> TargetApplicationIds):base(applicationId, InputResource){
            this.TargetApplicationIds=TargetApplicationIds;
        }
        public ICollection<string> TargetApplicationIds; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonSingle2Toggle {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonSingleToggle {0}(eApplicationID::{0}, {1}, {2});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputResource, 
                VectorOfApplicationIds(TargetApplicationIds, m)
                );

            return sb.ToString();
        }
    }

    public class PushButtonSingle2PwmSingleApplication :PushButtonSingleApplication{
        
        public PushButtonSingle2PwmSingleApplication(string applicationId, ushort InputResource, string TargetApplicationIds):base(applicationId, InputResource){
            this.TargetApplicationId=TargetApplicationIds;
        }
        public string TargetApplicationId; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonSingle2PwmSingle {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonSingle2PwmSingle {0}(eApplicationID::{0}, {1}, eApplicationID::{2});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputResource, 
                TargetApplicationId
                );

            return sb.ToString();
        }
    }

    public class PushButtonDual2BlindApplication :PushButtonDualApplication{
        
        public PushButtonDual2BlindApplication(string applicationId, ushort InputResource1, ushort InputResource2, string TargetApplicationIds):base(applicationId, InputResource1, InputResource2){
            this.TargetApplicationId=TargetApplicationIds;
        }
        public string TargetApplicationId; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonDual2Blind {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonDual2Blind {0}(eApplicationID::{0}, {1}, {2}, eApplicationID::{3});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputResource1,
                InputResource2,
                TargetApplicationId
                );

            return sb.ToString();
        }
    }

    public class PushButtonDual2PWMApplication :PushButtonDualApplication{
        
        public PushButtonDual2PWMApplication(string applicationId, ushort InputResource1, ushort InputResource2, string TargetApplicationIds):base(applicationId, InputResource1, InputResource2){
            this.TargetApplicationId=TargetApplicationIds;
        }
        public string TargetApplicationId; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonDual2PWM {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonDual2PWM {0}(eApplicationID::{0}, {1}, {2}, eApplicationID::{3});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputResource1,
                InputResource2,
                TargetApplicationId
                );

            return sb.ToString();
        }
    }
}
