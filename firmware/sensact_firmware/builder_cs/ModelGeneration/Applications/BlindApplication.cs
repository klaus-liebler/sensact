using System.Text;
using System.Text.RegularExpressions;
using Klli.Sensact.Model.Common.Nodes;

namespace Klli.Sensact.Model.Common.Applications
{
    public class BlindApplication : ActorApplication
    {
        public BlindApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort OutputResource1, ushort OutputResource2, RelayInterlockMode RelayMode, long UpTimeInSeconds, long DownTimeInSeconds):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.OutputResource1=OutputResource1;
            this.OutputResource2=OutputResource2;
            this.RelayMode=RelayMode;
            this.UpTimeInSeconds=UpTimeInSeconds;
            this.DownTimeInSeconds=DownTimeInSeconds;
            this.FullyCloseEvents=new();
            this.FullyOpenEvents=new();
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

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputResource1.ToString()))
            {
                return "OutputResource1 of "+ApplicationName;
            }
            if (usedOutputPins.Contains(OutputResource2.ToString()))
            {
                return "OutputResource2 of "+ApplicationName;
            }

            usedOutputPins.Add(OutputResource1.ToString());
            usedOutputPins.Add(OutputResource2.ToString());
            return string.Empty;
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            if (FullyCloseEvents.Count>0 || FullyOpenEvents.Count>0)
            {
                throw new NotImplementedException("Events of Blinds at "+ApplicationName);
            }
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("sensact::apps::cBlind {0}(eApplicationID::{0}, {1}, {2}, eRelayInterlockMode::{3}, {4}, {5})", ApplicationName, OutputResource1, OutputResource2, RelayMode, UpTimeInSeconds*1000, DownTimeInSeconds*1000);
            return sb.ToString();
        }
        

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("new Apps.BlindApplication(ApplicationId.{1}, '{2}', ctx)"+Environment.NewLine+Environment.NewLine, ApplicationId, ApplicationName, ApplicationDescription );
            return sb.ToString();
        }

        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("BLIND");
            }
        }
    }
}
