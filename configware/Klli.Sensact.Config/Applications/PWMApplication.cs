using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class PWMApplication : ActorApplication
    {
        public List<PwmPin> OutputRessources;
        public int MinimalOnLevel;
        public int InitialStoredTargetLevel;
        public bool LowMeansLampOn;
        public string StandbyController;

        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            HashSet<CommandType> ret = new HashSet<CommandType>()
            {
                CommandType.STOP, //beim Loslassen der Taste
                CommandType.START, //Beim Drücken der Taste
                CommandType.UP, //beim Hoch-Drück
                CommandType.DOWN, //beim Runter-Drück
                CommandType.STOP, //beim Loslassen-Lang
                CommandType.TOGGLE_SPECIAL, //beim Loslassen-Kurz
                CommandType.TOGGLE,
                CommandType.STEP_VERTICAL,
            };
            return ret;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            if(MinimalOnLevel<1)
            {
                MinimalOnLevel = 1;
            }
            if(MinimalOnLevel>Byte.MaxValue)
            {
                MinimalOnLevel = Byte.MaxValue;
            }
            if (InitialStoredTargetLevel<MinimalOnLevel || InitialStoredTargetLevel>255)
            {
                InitialStoredTargetLevel = Byte.MaxValue;
            }
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// PWM {0} (Dimmer )" + Environment.NewLine, ApplicationId);
            sb.Append(ResourcesInitializer("output", this.OutputRessources, m));
            sb.AppendFormat("sensact::cPWM {0}(\"{0}\", eApplicationID::{0}, {0}_output, {1}, {2}, {3}, {4}, eApplicationID::{5});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessources.Count, MinimalOnLevel, InitialStoredTargetLevel, LowMeansLampOn.ToString().ToLower(),StandbyController);
            return sb.ToString();
        }

        //PWM PushButton
        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("PWM__"+REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}
