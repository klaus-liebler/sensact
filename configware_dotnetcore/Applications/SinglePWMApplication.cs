using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class SinglePWMApplication : ActorApplication
    {
        public List<ushort> OutputRessources;
        public int MinimalOnLevel;
        public int InitialStoredTargetLevel;
        public bool LowMeansLampOn;
        public int AutoOffIntervalMsecs;
        public string StandbyController = "NO_APPLICATION";
        


        internal override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            foreach(ushort pwm in OutputRessources)
            {
                if (usedOutputPins.Contains(pwm.ToString()))
                {
                    return pwm.ToString();
                }
            }
            foreach (ushort pwm in OutputRessources)
            {
                usedOutputPins.Add(pwm.ToString());
            }
            return null;
        }

        [SensactCommandMethod]
        public override void OnONCommand(uint autoReturnToOffMsecs)
        {
            base.OnONCommand(autoReturnToOffMsecs);
        }

        [SensactCommandMethod]
        public override void OnOFFCommand(uint autoReturnToOffMsecs)
        {
            base.OnONCommand(autoReturnToOffMsecs);
        }

        public override void OnTOGGLECommand()
        {
            base.OnTOGGLECommand();
        }
        
        [SensactCommandMethod]
        public override void OnSET_VERTICAL_TARGETCommand(ushort target)
        {
            base.OnSET_VERTICAL_TARGETCommand(target);
        }

        [SensactCommandMethod]
        public override void OnSTEP_VERTICALCommand(short step)
        {
            base.OnSTEP_VERTICALCommand(step);
        }
        
        [SensactCommandMethod]
        public override void OnSTARTCommand()
        {
            base.OnSTARTCommand();
        }

        [SensactCommandMethod]
        public override void OnSTOPCommand()
        {
            base.OnSTOPCommand();
        }

        [SensactCommandMethod]
        public override void OnUPCommand(byte forced)
        {
            base.OnUPCommand(forced);
        }

        [SensactCommandMethod]
        public override void OnDOWNCommand(byte forced)
        {
            base.OnDOWNCommand(forced);
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
            AutoOffIntervalMsecs = Math.Max(0, AutoOffIntervalMsecs);
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// PWM {0} (Dimmer)" + Environment.NewLine, ApplicationId);
            sb.AppendFormat("sensactapps::cPWM {0}(eApplicationID::{0}, {1}, {2}, {3}, {4}, eApplicationID::{5}, {6});" + Environment.NewLine + Environment.NewLine, ApplicationId, VectorOfInOutIds(this.OutputRessources, m), MinimalOnLevel, InitialStoredTargetLevel, LowMeansLampOn.ToString().ToLower(),AutoOffIntervalMsecs, StandbyController);
            return sb.ToString();
        }

        //PWM PushButton
        internal override Regex AppIdRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("PWM__");
            }
        }
    }
}
