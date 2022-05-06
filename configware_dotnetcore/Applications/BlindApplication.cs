using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class BlindApplication : ActorApplication
    {
        public BlindApplication(ushort ApplicationId, string ApplicationName,  ushort OutputResource1, ushort OutputResource2, RelayInterlockMode RelayMode, long UpTimeInSeconds, long DownTimeInSeconds):base(ApplicationId, ApplicationName){
            this.OutputResource1=OutputResource1;
            this.OutputResource2=OutputResource2;
            this.RelayMode=RelayMode;
            this.UpTimeInSeconds=UpTimeInSeconds;
            this.DownTimeInSeconds=DownTimeInSeconds;
        }
        public ushort OutputResource1{get;}
        public ushort OutputResource2{get;}
        public RelayInterlockMode RelayMode{get;}
        public List<Event> FullyCloseEvents{get;}
        public List<Event> FullyOpenEvents{get;}
        public long UpTimeInSeconds{get;}
        public long DownTimeInSeconds{get;}
        

        [SensactCommandMethod]
        public override void OnUPCommand(byte forced)
        {

        }
         
        [SensactCommandMethod]
        public override void OnDOWNCommand(byte forced)
        {

        }

        [SensactCommandMethod]
        public override void OnSTOPCommand()
        {

        }
        
        [SensactCommandMethod]
        public override void OnSET_VERTICAL_TARGETCommand(ushort target)
        {
            
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputResource1.ToString()))
            {
                return "OutputResource1";
            }
            if (usedOutputPins.Contains(OutputResource2.ToString()))
            {
                return "OutputResource2";
            }

            usedOutputPins.Add(OutputResource1.ToString());
            usedOutputPins.Add(OutputResource2.ToString());
            return null;
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            if (FullyCloseEvents != null || FullyOpenEvents != null)
            {
                throw new NotImplementedException("Events of Blinds");
            }
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// Blind {0}"+Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::apps::cBlind {0}(eApplicationID::{0}, {1}, {2}, eRelayInterlockMode::{3}, {4}, {5});"+Environment.NewLine+Environment.NewLine, ApplicationId, OutputResource1, OutputResource2, RelayMode, UpTimeInSeconds*1000, DownTimeInSeconds*1000);
            return sb.ToString();
        }

        internal override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("BLIND");
            }
        }
    }
}
