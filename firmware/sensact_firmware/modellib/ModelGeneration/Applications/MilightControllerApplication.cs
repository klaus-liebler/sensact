using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class MilightControllerApplication : SensorApplication
    {
        // gesendete Kommandos: milightcontroller.cc
        public override IEnumerable<(ushort Target, CommandType Command)> SentCommands() => To(Light, CommandType.ON, CommandType.OFF, CommandType.SET_VERTICAL_TARGET).Concat(To(Blinds, CommandType.UP, CommandType.DOWN));

        /// <param name="Light">Lampe fuer ON/OFF/Dimmer-Rad</param>
        /// <param name="Blinds">Rolllaeden fuer die Tastengruppen 1..8 ("0" = hoch, "1" = runter); NO_APPLICATION = unbelegt</param>
        /// <param name="LightOnMsecs">Einschaltdauer bei ON</param>
        public MilightControllerApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort Light, IList<ushort> Blinds, uint LightOnMsecs):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.Light=Light;
            this.Blinds=Blinds;
            this.LightOnMsecs=LightOnMsecs;
        }
        public ushort Light{get;}
        public IList<ushort> Blinds{get;}
        public uint LightOnMsecs{get;}
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
            sb.AppendFormat("sensact::apps::cMilightController {0}(eApplicationID::{0}, eApplicationID::{1}, {2}, {3})",
                ApplicationName, m.GetNameFromId(Light), VectorOfApplicationIds(Blinds, m), LightOnMsecs);

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
