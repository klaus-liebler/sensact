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
                CommandType.TOGGLE_FILTER, CommandType.SEND_STATUS, CommandType.ON_FILTER
            };
            return ret;
        }

        public override void OnTOGGLE_FILTERCommand(ushort targetApplicationId)
        {
            base.OnTOGGLE_FILTERCommand(targetApplicationId);
        }
      

        public override void OnSEND_STATUSCommand()
        {
            base.OnSEND_STATUSCommand();
        }

        public override void OnON_FILTERCommand(ushort targetApplicationId, uint autoReturnToOffMsecs)
        {
            base.OnON_FILTERCommand(targetApplicationId, autoReturnToOffMsecs);
        }
        

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>() { EventType.STATUS };
        }

        public override string GenerateInitializer(ModelContainer m)
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
