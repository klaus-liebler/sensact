using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using Klli.Sensact.Config.Nodes;

namespace Klli.Sensact.Config.Applications
{
    public class PumpApplication : ActorApplication
    {
        public PumpApplication(ushort ApplicationId, string ApplicationName, ushort OutputRessource,  uint OnIntervalMsecs, uint OffIntervalMsecs):base(ApplicationId, ApplicationName){
            this.OutputRessource=OutputRessource;
            this.OnIntervalMsecs=OnIntervalMsecs;
            this.OffIntervalMsecs=OffIntervalMsecs;
        }
        public ushort OutputRessource{get;}

        public uint OnIntervalMsecs{get;}
        public uint OffIntervalMsecs{get;}
      

       

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessource.ToString()))
            {
                return "OutputRessource of "+ApplicationId;
            }
            usedOutputPins.Add(OutputRessource.ToString());
            return null;
        }



        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// Pump {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensactapps::cPump {0}(eApplicationID::{0}, {1}, {2}, {3});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessource, OnIntervalMsecs, OffIntervalMsecs);
            return sb.ToString();
        }

        internal override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("PUMP_");
            }
        }
    }
}
