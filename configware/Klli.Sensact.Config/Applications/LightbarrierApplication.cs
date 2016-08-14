using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class LightbarrierApplication : SensorApplication
    {

        public InputPin InputRessource;
        public string FinalTarget;
        public string BrightnessSensor;
        public bool ActiveSignalLevel;


        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>
            {
            };
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedPins)
        {
            if (usedPins.Contains(InputRessource.ToString()))
            {
                return "InputRessource";
            }
          
            usedPins.Add(InputRessource.ToString());
            return null;
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("LIBAR" + REGEX_FLOOR_ROOM_SUFFIX);
            }
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// Lightbarrier {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensact::cLightbarrier {0}(\"{0}\", eApplicationID::{0}, eInput::{1}, {2}, eApplicationID::{3}, eApplicationID::{4});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId, InputRessource, 
                ActiveSignalLevel.ToString().ToLower(),
                BrightnessSensor,
                FinalTarget);

            return sb.ToString();
        }
    }
}
