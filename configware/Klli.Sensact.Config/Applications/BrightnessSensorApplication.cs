using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class BrightnessSensorApplication : ActorApplication
    {
        public string SensorName;
        public string ToggleTarget;
        public int LimitForPassingToggle;

        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            HashSet<CommandType> ret = new HashSet<CommandType>()
            {
                CommandType.TOGGLE, CommandType.SEND_STATUS, CommandType.ON
            };
            return ret;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>() { EventType.STATUS };
        }

        public override string GenerateInitializer(Model m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// BrightnessSensor {0}" + Environment.NewLine, ApplicationId);
            //cBrightnessSensor::cBrightnessSensor(const char* name, eApplicationID id, drivers::cBH1750 *sensor, eApplicationID toggleTarget, uint16_t limitForPassingToggle)
            sb.AppendFormat("sensact::cBrightnessSensor {0}(\"{0}\", eApplicationID::{0}, &{1}, eApplicationID::{2}, {3});"+Environment.NewLine+Environment.NewLine, ApplicationId, SensorName, ToggleTarget, LimitForPassingToggle);
            return sb.ToString();
        }

        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("BSENS_.*");
            }
        }
    }
}
