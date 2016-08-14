using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class RGBW
    {
        public int R;
        public int G;
        public int B;
        public int W;
    }

    public class RgbwApplication : ActorApplication
    {
        public PwmPin OutputRessourceR;
        public PwmPin OutputRessourceG;
        public PwmPin OutputRessourceB;
        public PwmPin OutputRessourceW;
        public bool LowMeansLampOn;
        public string StandbyController="NO_APPLICATION";

        public override void OnSET_RGBWCommand(byte R, byte G, byte B, byte W)
        {
            Console.Write("");
        }

        public override void OnSET_SIGNALCommand(ushort signal)
        {
            base.OnSET_SIGNALCommand(signal);
        }

        public override void OnTOGGLECommand()
        {
            base.OnTOGGLECommand();
        }

        public override void OnSTEP_VERTICALCommand(short step)
        {
            Console.Write("");
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// RGBW {0} (Full Color Light)" + Environment.NewLine, ApplicationId);
            //sensact::cRgbw BATH("RGBWBATH", eApplicationID::BELL__DOOR, ePWMOutput::O1_01, ePWMOutput::O1_02, ePWMOutput::O1_03, ePWMOutput::NONE, false, (uint8_t*)MODEL::wellKnownRGBWColors, 2, eApplicationID::STDBY_XX_XXX_1);
            sb.AppendFormat("sensact::cRgbw {0}(\"{0}\", eApplicationID::{0}, ePWMOutput::{1}, ePWMOutput::{2}, ePWMOutput::{3}, ePWMOutput::{4}, {5}, (uint8_t*)MODEL::wellKnownRGBWColors, MODEL::wellKnownRGBWColorsCnt, eApplicationID::{6});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessourceR, OutputRessourceG, OutputRessourceB, OutputRessourceW, LowMeansLampOn.ToString().ToLower(), StandbyController);
            return sb.ToString();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedPins)
        {
            if (usedPins.Contains(OutputRessourceR.ToString()))
            {
                return "OutputRessourceR";
            }
            if (usedPins.Contains(OutputRessourceG.ToString()))
            {
                return "OutputRessourceG";
            }
            if (usedPins.Contains(OutputRessourceB.ToString()))
            {
                return "OutputRessourceB";
            }
            if (OutputRessourceW!=PwmPin.NONE && usedPins.Contains(OutputRessourceW.ToString()))
            {
                return "OutputRessourceR";
            }
            usedPins.Add(OutputRessourceR.ToString());
            usedPins.Add(OutputRessourceG.ToString());
            usedPins.Add(OutputRessourceB.ToString());
            if(OutputRessourceW!=PwmPin.NONE)
            {
                usedPins.Add(OutputRessourceW.ToString());
            }
            return null;
        }

        //PWM PushButton
        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("RGBW_"+REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}
