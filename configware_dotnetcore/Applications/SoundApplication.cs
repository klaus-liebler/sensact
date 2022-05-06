using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class SoundApplication : ActorApplication
    {

        public SoundApplication(ushort ApplicationId, string ApplicationName, byte initialVolume=100, ushort defaultSignalOnToggle=0):base(ApplicationId, ApplicationName){
            this.DefaultVolume=initialVolume;
            this.defaultSignalOnToggle=defaultSignalOnToggle;
        }
        public byte DefaultVolume{get;}
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

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// SOUND {0}" + Environment.NewLine, ApplicationId);
            //cBell DOORBELL("DOORBELL", eApplicationID::DOORBELL, &MODEL::volumeSchedule);
            sb.AppendFormat("sensact::apps::cSound {0}(eApplicationID::{0}, {1}, {2});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId, DefaultVolume, defaultSignalOnToggle);
            return sb.ToString();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return null;
        }

        internal override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("SOUND");
            }
        }
    }
}
