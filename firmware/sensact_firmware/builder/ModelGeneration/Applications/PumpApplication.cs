using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class PumpApplication : ActorApplication
    {
        public PumpApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort OutputRessource,  uint OnIntervalMsecs, uint OffIntervalMsecs):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.OutputRessource=OutputRessource;
            this.OnIntervalMsecs=OnIntervalMsecs;
            this.OffIntervalMsecs=OffIntervalMsecs;
        }
        public ushort OutputRessource{get;}

        public uint OnIntervalMsecs{get;}
        public uint OffIntervalMsecs{get;}
      

       

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessource.ToString()))
            {
                return "OutputRessource of "+ApplicationName;
            }
            usedOutputPins.Add(OutputRessource.ToString());
            return string.Empty;
        }



        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("sensact::apps::cPump {0}(eApplicationID::{0}, {1}, {2}, {3})", ApplicationName, OutputRessource, OnIntervalMsecs, OffIntervalMsecs);
            return sb.ToString();
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("PUMP_");
            }
        }
    }
}
