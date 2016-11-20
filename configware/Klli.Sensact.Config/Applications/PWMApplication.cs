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
        public string StandbyController = "NO_APPLICATION";
        public int AutoOffIntervalMsecs;

        public override void OnSTOPCommand()
        {
            base.OnSTOPCommand();
        }

        internal override string CheckAndAddUsedPins(HashSet<string> usedPins)
        {
            foreach(PwmPin pwm in OutputRessources)
            {
                if (usedPins.Contains(pwm.ToString()))
                {
                    return pwm.ToString();
                }
            }
            foreach (PwmPin pwm in OutputRessources)
            {
                usedPins.Add(pwm.ToString());
            }
            return null;
        }

        public override void OnSTARTCommand()
        {
            base.OnSTARTCommand();
        }

        public override void OnUPCommand(byte forced)
        {
            base.OnUPCommand(forced);
        }

        public override void OnDOWNCommand(byte forced)
        {
            base.OnDOWNCommand(forced);
        }

        public override void OnSTEP_VERTICALCommand(short step)
        {
            base.OnSTEP_VERTICALCommand(step);
        }

        public override void OnSET_VERTICAL_TARGETCommand(ushort target)
        {
            base.OnSET_VERTICAL_TARGETCommand(target);
        }

        public override void OnONCommand(uint autoReturnToOffMsecs)
        {
            base.OnONCommand(autoReturnToOffMsecs);
        }

        public override void OnTOGGLECommand()
        {
            base.OnTOGGLECommand();
        }



        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(ModelContainer m)
        {
            if(MinimalOnLevel<1)
            {
                MinimalOnLevel = 127;
            }
            if(MinimalOnLevel>Byte.MaxValue)
            {
                MinimalOnLevel = Byte.MaxValue;
            }
            if (InitialStoredTargetLevel<MinimalOnLevel || InitialStoredTargetLevel>255)
            {
                InitialStoredTargetLevel = Byte.MaxValue;
            }
            AutoOffIntervalMsecs = Math.Max(0, AutoOffIntervalMsecs);
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// PWM {0} (Dimmer )" + Environment.NewLine, ApplicationId);
            sb.Append(ResourcesInitializer("output", this.OutputRessources, m));
            sb.AppendFormat("sensact::cPWM {0}(\"{0}\", eApplicationID::{0}, {0}_output, {1}, {2}, {3}, {4}, eApplicationID::{5}, {6});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputRessources.Count, MinimalOnLevel, InitialStoredTargetLevel, LowMeansLampOn.ToString().ToLower(),StandbyController, AutoOffIntervalMsecs);
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
