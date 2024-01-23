using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{

    public class RgbwApplication : ActorApplication
    {
        public RgbwApplication(ushort ApplicationId, string ApplicationName,  string ApplicationDescription, ushort OutputRessourceR, ushort OutputRessourceG, ushort OutputRessourceB, ushort OutputRessourceW, uint AutoOffIntervalMsecs, ushort idOfStandbyController) : base(ApplicationId, ApplicationName, ApplicationDescription)
        {
            this.OutputRessourceR = OutputRessourceR;
            this.OutputRessourceG = OutputRessourceG;
            this.OutputRessourceB = OutputRessourceB;
            this.OutputRessourceW = OutputRessourceW;
            this.AutoOffIntervalMsecs = AutoOffIntervalMsecs;
            this.IdOfStandbyController = idOfStandbyController;
        }
        public ushort OutputRessourceR { get; }
        public ushort OutputRessourceG { get; }
        public ushort OutputRessourceB { get; }
        public ushort OutputRessourceW { get; }
        public ushort IdOfStandbyController { get; }
        public uint AutoOffIntervalMsecs { get; }

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

        public override string GenerateInitializer(ModelContainerForCodeGenerator m)
        {

            StringBuilder sb = new StringBuilder();
            sb.AFL("// RGBW {0} (Full Color Light)", ApplicationName);
            //sensact::cRgbw BATH("RGBWBATH", eApplicationID::BELL__DOOR, ePWMOutput::O1_01, ePWMOutput::O1_02, ePWMOutput::O1_03, ePWMOutput::NONE, false, (uint8_t*)MODEL::wellKnownRGBWColors, 2, eApplicationID::STDBY_XX_XXX_1);
            sb.AppendFormat("sensact::apps::cRgbw {0}(eApplicationID::{0}, {1}, {2}, {3}, {4}, {5}, eApplicationID::{6});" + Environment.NewLine + Environment.NewLine, ApplicationName, OutputRessourceR, OutputRessourceG, OutputRessourceB, OutputRessourceW, AutoOffIntervalMsecs, m.GetNameFromId(IdOfStandbyController));
            return sb.ToString();
        }

        public override string GenerateHTMLUserInterface(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessourceR.ToString()))
            {
                return "OutputRessourceR of Rgbw " + ApplicationName;
            }
            if (usedOutputPins.Contains(OutputRessourceG.ToString()))
            {
                return "OutputRessourceG of Rgbw " + ApplicationName;
            }
            if (usedOutputPins.Contains(OutputRessourceB.ToString()))
            {
                return "OutputRessourceB of Rgbw " + ApplicationName;
            }
            if (OutputRessourceW != ushort.MaxValue && usedOutputPins.Contains(OutputRessourceW.ToString()))
            {
                return "OutputRessourceW of Rgbw " + ApplicationName;
            }
            usedOutputPins.Add(OutputRessourceR.ToString());
            usedOutputPins.Add(OutputRessourceG.ToString());
            usedOutputPins.Add(OutputRessourceB.ToString());
            if (OutputRessourceW != ushort.MaxValue)
            {
                usedOutputPins.Add(OutputRessourceW.ToString());
            }
            return string.Empty;
        }

        //PWM PushButton
        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("RGBW_");
            }
        }
    }
}
