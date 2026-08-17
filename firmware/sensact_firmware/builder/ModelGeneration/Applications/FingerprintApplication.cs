using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class FingerprintApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort TargetApplication1, ushort TargetApplication2, ushort TargetApplication3):SensorApplication(ApplicationId, ApplicationName, ApplicationDescription)
    {
        
        public ushort TargetApplication{get;}



        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>
            {
                EventType.FINGER_DETECTED,
                EventType.FINGER_NOT_DETECTED,
                EventType.FINGER_ADDED,
            };
        }



        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("FINGR");
            }
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new();
            sb.AppendFormat("sensact::apps::cFingerprint {0}(eApplicationID::{0}, eApplicationID::{1}, eApplicationID::{2}, eApplicationID::{3})", 
                m.GetNameFromId(ApplicationId),
                m.GetNameFromId(TargetApplication1),
                m.GetNameFromId(TargetApplication2),
                m.GetNameFromId(TargetApplication3)
                );

            return sb.ToString();
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return string.Empty;
        }
    }
}
