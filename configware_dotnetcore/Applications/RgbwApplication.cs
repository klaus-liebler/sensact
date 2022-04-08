using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{

    public class RgbwApplication : ActorApplication
    {
         public RgbwApplication(string ApplicationId, ushort OutputRessourceR, ushort OutputRessourceG, ushort OutputRessourceB, ushort OutputRessourceW, uint AutoOffIntervalMsecs, string idOfStandbyController="NO_APPLICATION"):base(ApplicationId){
            this.OutputRessourceR=OutputRessourceR;
            this.OutputRessourceG=OutputRessourceG;
            this.OutputRessourceB=OutputRessourceB;
            this.OutputRessourceW=OutputRessourceW;
            this.AutoOffIntervalMsecs=AutoOffIntervalMsecs;
            this.IdOfStandbyController=idOfStandbyController;
        }
        public ushort OutputRessourceR{get;}
        public ushort OutputRessourceG{get;}
        public ushort OutputRessourceB{get;}
        public ushort OutputRessourceW{get;}
        public string IdOfStandbyController{get;}
        public uint AutoOffIntervalMsecs{get;}

        public override void OnSET_RGBWCommand(byte R, byte G, byte B, byte W)
        {
            
        }
        [SensactCommandMethod]
        public override void OnSET_SIGNALCommand(ushort signal)
        {
            base.OnSET_SIGNALCommand(signal);
        }
        [SensactCommandMethod]
        public override void OnTOGGLECommand()
        {
            base.OnTOGGLECommand();
        }
        [SensactCommandMethod]
        public override void OnSTEP_VERTICALCommand(short step)
        {
            base.OnSTEP_VERTICALCommand(step);
        }
        [SensactCommandMethod]
        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// RGBW {0} (Full Color Light)" + Environment.NewLine, ApplicationId);
            //sensact::cRgbw BATH("RGBWBATH", eApplicationID::BELL__DOOR, ePWMOutput::O1_01, ePWMOutput::O1_02, ePWMOutput::O1_03, ePWMOutput::NONE, false, (uint8_t*)MODEL::wellKnownRGBWColors, 2, eApplicationID::STDBY_XX_XXX_1);
            sb.AppendFormat("sensact::apps::cRgbw {0}(eApplicationID::{0}, {1}, {2}, {3}, {4}, {5}, eApplicationID::{6});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessourceR, OutputRessourceG, OutputRessourceB, OutputRessourceW, AutoOffIntervalMsecs, IdOfStandbyController);
            return sb.ToString();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
       {
            if (usedOutputPins.Contains(OutputRessourceR.ToString()))
            {
                return "OutputRessourceR of Rgbw "+ApplicationId;
            }
            if (usedOutputPins.Contains(OutputRessourceG.ToString()))
            {
                return "OutputRessourceG of Rgbw "+ApplicationId;
            }
            if (usedOutputPins.Contains(OutputRessourceB.ToString()))
            {
                return "OutputRessourceB of Rgbw "+ApplicationId;
            }
            if (OutputRessourceW!=ushort.MaxValue && usedOutputPins.Contains(OutputRessourceW.ToString()))
            {
                return "OutputRessourceW of Rgbw "+ApplicationId;
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
                return FLOOR_ROOM_Regex("RGBW_");
            }
        }
    }
}
