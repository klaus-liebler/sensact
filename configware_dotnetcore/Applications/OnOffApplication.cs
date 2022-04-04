using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using Klli.Sensact.Config.Nodes;

namespace Klli.Sensact.Config.Applications
{
    public class OnOffApplication : ActorApplication
    {
        public ushort OutputRessource;
        public PowerState InitialPowerState;
        public uint AutoOffIntervalMsecs;
        public List<Event> ToggleEvents;
        public List<Event> OffEvents;
        public List<Event> OnEvents;

       

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessource.ToString()))
            {
                return "OutputRessource";
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
            sb.AppendFormat("sensactapps::cOnOff {0}(eApplicationID::{0}, {1}, {2}, ePowerState::{3}, {4});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessource, InitialPowerState.ToString(), AutoOffIntervalMsecs);
            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("POWIT");
            }
        }
    }
}
