using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class RCEventApplication : SensorApplication
    {
        public RCEventApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription):base(ApplicationId, ApplicationName, ApplicationDescription){

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
                EventType.PRESSED,
                EventType.PRESSED_SHORT_AND_HOLD,
                EventType.PRESSED_MEDIUM_AND_HOLD,
                EventType.RELEASED,
                EventType.RELEASED_SHORT,
                EventType.RELEASED_MEDIUM,
                EventType.RELEASED_LONG
            };
        }



        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("RCEVT");
            }
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {

            StringBuilder sb = new StringBuilder();
            sb.Append(CommandInitializer("OnPressed", CommandsOnPressed, m));
            sb.Append(CommandInitializer("OnReleased", CommandsOnReleased, m));
            sb.Append(CommandInitializer("OnReleasedShort", CommandsOnReleasedShort, m));
            sb.Append(CommandInitializer("OnPressedShortAndHold", CommandsOnPressedShortAndHold, m));
            sb.Append(CommandInitializer("OnReleasedLong", CommandsOnReleasedLong, m));

            sb.AppendFormat("sensact::apps::cRCEvent {0}(eApplicationID::{0}, {1}, {0}_OnPressed, {2}, {0}_OnReleased, {3}, {0}_OnReleasedShort, {4}, {0}_OnPressedShortAndHold, {5}, {0}_OnReleasedLong, {6})", 
                ApplicationName, EventCode, 
                CommandsOnPressed == null ? 0 : CommandsOnPressed.Count,
                CommandsOnReleased == null ? 0 : CommandsOnReleased.Count,
                CommandsOnReleasedShort == null ? 0 : CommandsOnReleasedShort.Count, 
                CommandsOnPressedShortAndHold==null ? 0:CommandsOnPressedShortAndHold.Count,
                CommandsOnReleasedLong == null ? 0 : CommandsOnReleasedLong.Count);

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
