using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class SoundApplication : ActorApplication
    {

        public ushort StandbyOutput;
        public string NameOfVoulumeScheduleOrNull= "&MODEL::volumeSchedule";
        public int DefaultVolume = 30;

        public override void OnSET_SIGNALCommand(ushort signal)
        {
            base.OnSET_SIGNALCommand(signal);
        }

        public override void OnSTARTCommand()
        {
            base.OnSTARTCommand();
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
            sb.AppendFormat("sensactapps::cSound {0}(eApplicationID::{0}, {1}, {2}, {3});" + Environment.NewLine + Environment.NewLine, 
                ApplicationId, StandbyOutput, NameOfVoulumeScheduleOrNull!=null?NameOfVoulumeScheduleOrNull: "NULL", DefaultVolume);
            return sb.ToString();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(StandbyOutput.ToString()))
            {
                return nameof(StandbyOutput);
            }

            usedOutputPins.Add(StandbyOutput.ToString());
            return null;
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("SOUND");
            }
        }
    }
}
