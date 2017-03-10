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
        public ushort OutputRessourceR;
        public ushort OutputRessourceG;
        public ushort OutputRessourceB;
        public ushort OutputRessourceW;
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
            sb.AppendFormat("sensact::cRgbw {0}(\"{0}\", eApplicationID::{0}, {1}, {2}, {3}, {4}, {5}, (uint8_t*)MODEL::wellKnownRGBWColors, MODEL::wellKnownRGBWColorsCnt, eApplicationID::{6});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessourceR, OutputRessourceG, OutputRessourceB, OutputRessourceW, LowMeansLampOn.ToString().ToLower(), StandbyController);
            return sb.ToString();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
       {
            if (usedOutputPins.Contains(OutputRessourceR.ToString()))
            {
                return "OutputRessourceR";
            }
            if (usedOutputPins.Contains(OutputRessourceG.ToString()))
            {
                return "OutputRessourceG";
            }
            if (usedOutputPins.Contains(OutputRessourceB.ToString()))
            {
                return "OutputRessourceB";
            }
            if (OutputRessourceW!=ushort.MaxValue && usedOutputPins.Contains(OutputRessourceW.ToString()))
            {
                return "OutputRessourceR";
            }
            usedOutputPins.Add(OutputRessourceR.ToString());
            usedOutputPins.Add(OutputRessourceG.ToString());
            usedOutputPins.Add(OutputRessourceB.ToString());
            if(OutputRessourceW!= ushort.MaxValue)
            {
                usedOutputPins.Add(OutputRessourceW.ToString());
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
