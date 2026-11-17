using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class FingerprintApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort TargetApplication1, ushort TargetApplication2, ushort TargetApplication3):SensorApplication(ApplicationId, ApplicationName, ApplicationDescription)
    {
        // gesendete Kommandos: fingerprint.hh
        public override IEnumerable<(ushort Target, CommandType Command)> SentCommands() => To([Target1, Target2, Target3], CommandType.ON);

        
        public ushort TargetApplication{get;}
        public ushort Target1 { get; } = TargetApplication1;
        public ushort Target2 { get; } = TargetApplication2;
        public ushort Target3 { get; } = TargetApplication3;



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
                m.GetNameFromId(Target1),
                m.GetNameFromId(Target2),
                m.GetNameFromId(Target3)
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
