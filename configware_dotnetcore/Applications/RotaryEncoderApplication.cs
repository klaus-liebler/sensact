using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class RotaryEncoderApplication : SensorApplication
    {
        public List<Command> CommandsOnPressed;
        public List<Command> CommandsOnShortReleased;
        public List<Command> CommandsOnLongReleased;
        public List<Command> CommandsOnTurned;
        public RotaryEncoder InputRotaryRessource;
        public ushort InputPushRessource;


        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputPushRessource.ToString()))
            {
                return "InputPushRessource";
            }
            usedInputPins.Add(InputPushRessource.ToString());
            return null;
        }


        public override HashSet<EventType> ICanSendTheseEvents()
        {
            HashSet<EventType> ret = new HashSet<EventType>()
            {
                EventType.PRESSED,
                EventType.RELEASED,
                EventType.RELEASED_SHORT,
                EventType.RELEASED_MEDIUM
            };
            return ret;
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            //Command op_a1[1] = {{eApplicationID::POWIT_EG_WOZ_L1, eCommandType::BACKWARD }};
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// RotaryEncoder {0}" + Environment.NewLine, ApplicationId);
            sb.Append(CommandInitializer("OnPressed", CommandsOnPressed, m));
            sb.Append(CommandInitializer("OnShortReleased", CommandsOnShortReleased, m));
            sb.Append(CommandInitializer("OnLongReleased", CommandsOnLongReleased, m));
            sb.Append(CommandInitializer("OnTurned", CommandsOnTurned, m));
            HashSet<EventType> locEvts = new HashSet<EventType>();
            m.id2localEvents.TryGetValue(ApplicationId, out locEvts);
            HashSet<EventType> busEvts = new HashSet<EventType>();
            m.id2busEvents.TryGetValue(ApplicationId, out busEvts);

            sb.Append(EventInitializer("Local", locEvts, m));
            sb.Append(EventInitializer("Bus", busEvts, m));
            //homecontrol::cRotaryEncoder ROT_G0S0(eApplicationID::ROT_G0S0, sensact::eRotaryEncoder::ROTARYENCODER_1, sensact::eInput::I00, ROT_LocalEvents, 0, PUSHB_EG_WOZ_G0S0_BusEvents, 0, PUSHB_EG_WOZ_G0S0_OnPressed, 1, PUSHB_EG_WOZ_G0S0_OnShortReleased, 1, PUSHB_EG_WOZ_G0S0_OnLongReleased, 1, ROT_OnTurned, 1);

            sb.AppendFormat("sensactapps::cROTAR {0}(eApplicationID::{0}, eRotaryEncoder::{1}, {2}, {0}_LocalEvents, {3}, {0}_BusEvents, {4}, {0}_OnPressed, {5}, {0}_OnShortReleased, {6}, {0}_OnLongReleased, {7}, {0}_OnTurned, {8} );" + Environment.NewLine + Environment.NewLine, ApplicationId, InputRotaryRessource, InputPushRessource, locEvts == null ? 0 : locEvts.Count, busEvts == null ? 0 : busEvts.Count, CommandsOnPressed == null ? 0 : CommandsOnPressed.Count, CommandsOnShortReleased == null ? 0 : CommandsOnShortReleased.Count, CommandsOnLongReleased == null ? 0 :CommandsOnLongReleased.Count, CommandsOnTurned == null ? 0 : CommandsOnTurned.Count);

            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("ROTAR");
            }
        }
    }
}
