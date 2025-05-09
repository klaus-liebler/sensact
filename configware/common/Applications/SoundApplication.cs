﻿using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class SoundApplication : ActorApplication
    {

        public SoundApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort initialVolume=65535, ushort defaultSignalOnToggle=0):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.DefaultVolume=initialVolume;
            this.defaultSignalOnToggle=defaultSignalOnToggle;
        }
        public ushort DefaultVolume{get;}
        public ushort defaultSignalOnToggle{get;}
        
        [SensactCommandMethod]
        public override void OnTOGGLECommand()
        {
            base.OnTOGGLECommand();
        }

        
        [SensactCommandMethod]
        public override void OnSET_SIGNALCommand(ushort signal)
        {
            base.OnSET_SIGNALCommand(signal);
        }
        [SensactCommandMethod]
        public override void OnSTARTCommand()
        {
            base.OnSTARTCommand();
        }

        [SensactCommandMethod]
        public override void OnOFFCommand(uint autoReturnToOnMsecs)
        {
            base.OnOFFCommand(autoReturnToOnMsecs);
        }

        [SensactCommandMethod]
        public override void OnSTOPCommand()
        {
            base.OnSTARTCommand();
        }

        [SensactCommandMethod]
        public override void OnSET_VERTICAL_TARGETCommand(ushort target)
        {
            base.OnSET_VERTICAL_TARGETCommand(target);
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendFormat("sensact::apps::cSound {0}(eApplicationID::{0}, {1}, {2})", 
                ApplicationName, DefaultVolume, defaultSignalOnToggle);
            return sb.ToString();
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return string.Empty;
        }

        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("SOUND");
            }
        }
    }
}
