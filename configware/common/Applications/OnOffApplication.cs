using System.Text;
using System.Text.RegularExpressions;
using Klli.Sensact.Model.Common.Nodes;

namespace Klli.Sensact.Model.Common.Applications
{
    public class OnOffApplication : ActorApplication
    {
        public OnOffApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort OutputRessource, PowerState InitialPowerState, uint AutoOffIntervalMsecs):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.OutputRessource=OutputRessource;
            this.InitialPowerState=InitialPowerState;
            this.AutoOffIntervalMsecs=AutoOffIntervalMsecs;
            this.ToggleEvents=new();
            this.OnEvents=new();
            this.OffEvents=new();
        }
        public ushort OutputRessource{get;}
        public PowerState InitialPowerState{get;}
        public uint AutoOffIntervalMsecs{get;}
        public List<Event> ToggleEvents{get;}
        public List<Event> OffEvents{get;}
        public List<Event> OnEvents{get;}

       

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessource.ToString()))
            {
                return "OutputRessource of "+ApplicationName;
            }
            usedOutputPins.Add(OutputRessource.ToString());
            return String.Empty;
        }

        [SensactCommandMethod]
        public override void OnTOGGLE_SPECIALCommand()
        {

        }

        [SensactCommandMethod]
        public override void OnTOGGLECommand()
        {

        }

        [SensactCommandMethod]
        public override void OnONCommand(uint autoReturnToOffMsecs)
        {

        }

        [SensactCommandMethod]
        public override void OnOFFCommand(uint autoReturnToOnMsecs)
        {

        }

        [SensactCommandMethod]
        public override void OnHEARTBEATCommand(uint autoReturnToOnMsecs)
        {

        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            if (OffEvents.Count >0 || OnEvents.Count>0  || ToggleEvents.Count>0)
            {
                throw new NotImplementedException("Events of OnOffApplication "+ ApplicationName);
            }
            
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("sensact::apps::cOnOff {0}(eApplicationID::{0}, {1}, ePowerState::{2}, {3})", ApplicationName, OutputRessource, InitialPowerState.ToString(), AutoOffIntervalMsecs);
            return sb.ToString();
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("new Apps.OnOffApplication(ApplicationId.{1}, '{2}', ctx)", ApplicationId, ApplicationName, ApplicationDescription );
            return sb.ToString();
        }

        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("(POWIT)|(STDBY)");
            }
        }
    }
}
