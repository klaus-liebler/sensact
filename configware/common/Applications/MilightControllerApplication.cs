using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class MilightControllerApplication : SensorApplication
    {
        public MilightControllerApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription):base(ApplicationId, ApplicationName, ApplicationDescription){

        }
        public List<Command> CommandsOnPressed=new();
        public List<Command> CommandsOnReleased=new();
        public List<Command> CommandsOnReleasedShort=new();
        public List<Command> CommandsOnPressedShortAndHold=new();
        public List<Command> CommandsOnReleasedLong=new();
        public int EventCode=0;


        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>
            {
            };
        }



        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("REMOT");
            }
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {

            StringBuilder sb = new StringBuilder(); 
            sb.AppendFormat("sensact::apps::cMilightController {0}(eApplicationID::{0})", 
                ApplicationName, EventCode);

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
