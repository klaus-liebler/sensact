using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public abstract class PushButtonApplication : SensorApplication
    {
        protected PushButtonApplication(ushort ApplicationId, string ApplicationName):base(ApplicationId, ApplicationName){}
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

        internal override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("PUSHB");
            }
        }
    }
    
    public abstract class PushButtonSingleApplication : PushButtonApplication
    {
        protected PushButtonSingleApplication(ushort ApplicationId, string ApplicationName, ushort InputResource):base(ApplicationId, ApplicationName){
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
        protected PushButtonDualApplication(ushort ApplicationId, string ApplicationName, ushort InputResource1, ushort InputResource2):base(ApplicationId, ApplicationName){
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
        
        public PushButtonSingle2ToggleApplication(ushort ApplicationId, string ApplicationName, ushort InputResource, ICollection<ushort> TargetApplicationIds):base(ApplicationId, ApplicationName, InputResource){
            this.TargetApplicationIds=TargetApplicationIds;
        }

        public class JSON{

        }

        public override void AddJSONDescriptionToList(IList<object> list){
            list.Add(new JSONAppDescriptionMultipleTarget(ApplicationId, ApplicationName, this.GetType().Name, TargetApplicationIds));
        }

        public ICollection<ushort> TargetApplicationIds; 
        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// cPushbuttonSingle2Toggle {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cPushbuttonSingle2Toggle {0}(eApplicationID::{0}, {1}, {2});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId,
                InputResource, 
                VectorOfApplicationIds(TargetApplicationIds, m)
                );

            return sb.ToString();
        }
    }

    public class PushButtonSingle2PwmSingleApplication :PushButtonSingleApplication{
        
        public PushButtonSingle2PwmSingleApplication(ushort ApplicationId, string ApplicationName, ushort InputResource, ushort TargetApplicationId):base(ApplicationId, ApplicationName, InputResource){
            this.TargetApplicationId=TargetApplicationId;
        }
        public ushort TargetApplicationId; 
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
        
        public PushButtonDual2BlindApplication(ushort ApplicationId, string ApplicationName, ushort InputResource1, ushort InputResource2, ushort TargetApplicationId):base(ApplicationId, ApplicationName, InputResource1, InputResource2){
            this.TargetApplicationId=TargetApplicationId;
        }
        public ushort TargetApplicationId; 
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
        
        public PushButtonDual2PWMApplication(ushort ApplicationId, string ApplicationName, ushort InputResource1, ushort InputResource2, ushort TargetApplicationIds):base(ApplicationId, ApplicationName, InputResource1, InputResource2){
            this.TargetApplicationId=TargetApplicationIds;
        }
        public ushort TargetApplicationId; 
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
