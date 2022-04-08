using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class RCEventApplication : SensorApplication
    {
        public RCEventApplication(string ApplicationID):base(ApplicationID){

        }
        public List<Command> CommandsOnPressed;
        public List<Command> CommandsOnReleased;
        public List<Command> CommandsOnReleasedShort;
        public List<Command> CommandsOnPressedShortAndHold;
        public List<Command> CommandsOnReleasedLong;
        public int EventCode;


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



        internal override Regex AppIdRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("RCEVT");
            }
        }

        public override string GenerateInitializer(ModelContainer m)
        {

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// RCEvent {0}" + Environment.NewLine, ApplicationId);
            sb.Append(CommandInitializer("OnPressed", CommandsOnPressed, m));
            sb.Append(CommandInitializer("OnReleased", CommandsOnReleased, m));
            sb.Append(CommandInitializer("OnReleasedShort", CommandsOnReleasedShort, m));
            sb.Append(CommandInitializer("OnPressedShortAndHold", CommandsOnPressedShortAndHold, m));
            sb.Append(CommandInitializer("OnReleasedLong", CommandsOnReleasedLong, m));

            sb.AppendFormat("sensactapps::cRCEvent {0}(eApplicationID::{0}, {1}, {0}_OnPressed, {2}, {0}_OnReleased, {3}, {0}_OnReleasedShort, {4}, {0}_OnPressedShortAndHold, {5}, {0}_OnReleasedLong, {6});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId, EventCode, 
                CommandsOnPressed == null ? 0 : CommandsOnPressed.Count,
                CommandsOnReleased == null ? 0 : CommandsOnReleased.Count,
                CommandsOnReleasedShort == null ? 0 : CommandsOnReleasedShort.Count, 
                CommandsOnPressedShortAndHold==null ? 0:CommandsOnPressedShortAndHold.Count,
                CommandsOnReleasedLong == null ? 0 : CommandsOnReleasedLong.Count);

            return sb.ToString();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return null;
        }
    }
}
