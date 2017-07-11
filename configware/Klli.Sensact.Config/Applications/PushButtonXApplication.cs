using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class PushButtonXApplication : SensorApplication
    {
        public List<Command> CommandsOnPressed;
        public List<Command> CommandsOnReleased;
        public List<Command> CommandsOnReleasedShort;
        public List<Command> CommandsOnPressedShortAndHold;
        public List<Command> CommandsOnReleasedLong;
        public List<Command> CommandsOnDoubleclick;
        public ushort InputRessource;


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
                return new Regex("PUSHB" + REGEX_FLOOR_ROOM_SUFFIX);
            }
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputRessource.ToString()))
            {
                return "InputRessource";
            }

            usedInputPins.Add(InputRessource.ToString());
            return null;
        }

        public override string GenerateInitializer(ModelContainer m)
        {

            //Command op_a1[1] = {{eApplicationID::POWIT_EG_WOZ_L1, eCommandType::BACKWARD }};
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// PushButtonX {0}" + Environment.NewLine, ApplicationId);
            sb.Append(CommandInitializer("OnPressed", CommandsOnPressed, m));
            sb.Append(CommandInitializer("OnReleased", CommandsOnReleased, m));
            sb.Append(CommandInitializer("OnReleasedShort", CommandsOnReleasedShort, m));
            sb.Append(CommandInitializer("OnPressedShortAndHold", CommandsOnPressedShortAndHold, m));
            sb.Append(CommandInitializer("OnReleasedLong", CommandsOnReleasedLong, m));
            sb.Append(CommandInitializer("OnDoubleclick", CommandsOnDoubleclick, m));
            HashSet<EventType> locEvts = new HashSet<EventType>();
            m.id2localEvents.TryGetValue(ApplicationId, out locEvts);
            HashSet<EventType> busEvts = new HashSet<EventType>();
            m.id2busEvents.TryGetValue(ApplicationId, out busEvts);

            sb.Append(EventInitializer("Local", locEvts, m));
            sb.Append(EventInitializer("Bus", busEvts, m));
            sb.AppendFormat("sensact::cPushbuttonX {0}(eApplicationID::{0}, {1}, {0}_LocalEvents, {2}, {0}_BusEvents, {3}, {0}_OnPressed, {4}, {0}_OnReleased, {5}, {0}_OnReleasedShort, {6}, {0}_OnPressedShortAndHold, {7}, {0}_OnReleasedLong, {8}, {0}_OnDoubleclick, {9});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId, InputRessource, 
                locEvts == null ? 0 : locEvts.Count, 
                busEvts == null ? 0 : busEvts.Count, 
                CommandsOnPressed == null ? 0 : CommandsOnPressed.Count,
                CommandsOnReleased == null ? 0 : CommandsOnReleased.Count,
                CommandsOnReleasedShort == null ? 0 : CommandsOnReleasedShort.Count, 
                CommandsOnPressedShortAndHold==null ? 0:CommandsOnPressedShortAndHold.Count,
                CommandsOnReleasedLong == null ? 0 : CommandsOnReleasedLong.Count,
                CommandsOnDoubleclick == null ? 0:CommandsOnDoubleclick.Count);

            return sb.ToString();
        }
    }
}
