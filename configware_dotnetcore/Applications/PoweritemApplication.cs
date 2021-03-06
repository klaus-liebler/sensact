using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class PoweritemApplication : ActorApplication
    {
        public ushort OutputRessource;
        public uint AutoOffIntervalMsecs;
        public uint AutoOnIntervalMsecs;
        public List<Event> ToggleEvents;
        public List<Event> OffEvents;
        public List<Event> OnEvents;

        [SensactCommandMethod]
        public override void OnTOGGLECommand()
        {

        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            if (usedOutputPins.Contains(OutputRessource.ToString()))
            {
                return "OutputRessource";
            }
            usedOutputPins.Add(OutputRessource.ToString());
            return null;
        }

        [SensactCommandMethod]
        public override void OnTOGGLE_SPECIALCommand()
        {

        }

        [SensactCommandMethod]
        public override void OnONCommand(uint autoReturnToOffMsecs)
        {

        }

        [SensactCommandMethod]
        public override void OnOFFCommand(uint autoReturnToOnMsecs)
        {

        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// POWIT {0}" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensactapps::cPoweritem {0}(eApplicationID::{0}, {1}, {2}, {3});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessource, AutoOffIntervalMsecs, AutoOnIntervalMsecs);
            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("POWIT" + REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}
