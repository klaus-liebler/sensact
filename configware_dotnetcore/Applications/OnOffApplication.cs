using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using Klli.Sensact.Config.Nodes;

namespace Klli.Sensact.Config.Applications
{
    public class OnOffApplication : ActorApplication
    {
        public OnOffApplication(ushort ApplicationId, string ApplicationName, ushort OutputRessource, PowerState InitialPowerState, uint AutoOffIntervalMsecs):base(ApplicationId, ApplicationName){
            this.OutputRessource=OutputRessource;
            this.InitialPowerState=InitialPowerState;
            this.AutoOffIntervalMsecs=AutoOffIntervalMsecs;
        }
        public ushort OutputRessource{get;}
        public PowerState InitialPowerState{get;}
        public uint AutoOffIntervalMsecs{get;}
        public List<Event> ToggleEvents{get;}
        public List<Event> OffEvents{get;}
        public List<Event> OnEvents{get;}

       

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessource.ToString()))
            {
                return "OutputRessource of "+ApplicationId;
            }
            usedOutputPins.Add(OutputRessource.ToString());
            return null;
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

        public override string GenerateInitializer(ModelContainer m)
        {
            if (OffEvents != null || OnEvents != null || ToggleEvents!=null)
            {
                throw new NotImplementedException("Events of OnOffApplication "+ ApplicationId);
            }
            
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// ONOFF {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cOnOff {0}(eApplicationID::{0}, {1}, ePowerState::{2}, {3});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessource, InitialPowerState.ToString(), AutoOffIntervalMsecs);
            return sb.ToString();
        }

        internal override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("(POWIT)|(STDBY)");
            }
        }
    }
}
