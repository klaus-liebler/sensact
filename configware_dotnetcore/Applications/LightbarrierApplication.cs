using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class LightbarrierApplication : SensorApplication
    {
        public LightbarrierApplication(string ApplicationId, ushort InputRessource, string FinalTarget, string BrightnessSensor, bool ActiveSignalLevel):base(ApplicationId){
            this.InputRessource=InputRessource;
            this.FinalTarget=FinalTarget;
            this.BrightnessSensor=BrightnessSensor;
            this.ActiveSignalLevel=ActiveSignalLevel;
        }
        public ushort InputRessource{get;}
        public string FinalTarget{get;}
        public string BrightnessSensor{get;}
        public bool ActiveSignalLevel{get;}


        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>
            {
            };
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputRessource.ToString()))
            {
                return "InputRessource";
            }
            usedInputPins.Add(InputRessource.ToString());
            return null;
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("LIBAR");
            }
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// Lightbarrier {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensactapps::cLightbarrier {0}(eApplicationID::{0}, {1}, {2}, eApplicationID::{3}, eApplicationID::{4});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId, InputRessource, 
                ActiveSignalLevel.ToString().ToLower(),
                BrightnessSensor,
                FinalTarget);

            return sb.ToString();
        }
    }
}
