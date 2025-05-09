﻿using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class LightbarrierApplication : SensorApplication
    {
        public LightbarrierApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort InputRessource, ushort FinalTarget, ushort BrightnessSensor, bool ActiveSignalLevel):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.InputRessource=InputRessource;
            this.FinalTarget=FinalTarget;
            this.BrightnessSensor=BrightnessSensor;
            this.ActiveSignalLevel=ActiveSignalLevel;
        }
        public ushort InputRessource{get;}
        public ushort FinalTarget{get;}
        public ushort BrightnessSensor{get;}
        public bool ActiveSignalLevel{get;}


        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>
            {
            };
        }

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedInputPins.Contains(InputRessource.ToString()))
            {
                return "InputRessource of "+ApplicationName;
            }
            usedInputPins.Add(InputRessource.ToString());
            return String.Empty;
        }

        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("LIBAR");
            }
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("sensact::apps::cLightbarrier {0}(eApplicationID::{0}, {1}, {2}, eApplicationID::{3}, eApplicationID::{4})", 
                ApplicationName, InputRessource, 
                ActiveSignalLevel.ToString().ToLower(),
                BrightnessSensor,
                m.GetNameFromId(FinalTarget));

            return sb.ToString();
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }
    }
}
