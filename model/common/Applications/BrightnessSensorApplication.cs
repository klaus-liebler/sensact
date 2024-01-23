using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class BrightnessSensorApplication : ActorApplication
    {
        public BrightnessSensorApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, string sensorName, ushort ToggleTarget, int LimitForPassingToggle):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.SensorName=sensorName;
            this.ToggleTarget=ToggleTarget;
            this.LimitForPassingToggle=LimitForPassingToggle;
        }
        public string SensorName;
        public ushort ToggleTarget;
        public int LimitForPassingToggle;

        public override void OnTOGGLE_FILTERCommand(ushort targetApplicationId)
        {
            base.OnTOGGLE_FILTERCommand(targetApplicationId);
        }
      

        public override void OnSEND_STATUSCommand()
        {
            base.OnSEND_STATUSCommand();
        }

        public override void OnON_FILTERCommand(ushort targetApplicationId, uint autoReturnToOffMsecs)
        {
            base.OnON_FILTERCommand(targetApplicationId, autoReturnToOffMsecs);
        }

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return String.Empty;
        }


        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>() { EventType.STATUS };
        }

        public override string GenerateInitializer(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// BrightnessSensor {0}" + Environment.NewLine, ApplicationName);
            //cBrightnessSensor::cBrightnessSensor(const char* name, eApplicationID id, drivers::cBH1750 *sensor, eApplicationID toggleTarget, uint16_t limitForPassingToggle)
            sb.AppendFormat("sensactapps::cBrightnessSensor {0}(eApplicationID::{0}, &{1}, eApplicationID::{2}, {3});"+Environment.NewLine+Environment.NewLine, ApplicationName, SensorName, ToggleTarget, LimitForPassingToggle);
            return sb.ToString();
        }

        public override string GenerateHTMLUserInterface(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override Regex AppNameRegex
        {
            get
            {
                return new Regex("BSENS_.*");
            }
        }
    }
}
