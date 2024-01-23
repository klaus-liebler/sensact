using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class SinglePWMApplication:ActorApplication{
        public SinglePWMApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ISet<ushort> OutputResources,  int MinimalOnLevel, int InitialStoredTargetLevel, uint AutoOffIntervalMsecs, ushort StandbyController):base(ApplicationId, ApplicationName, ApplicationDescription){
            if(MinimalOnLevel<1)
            {
                MinimalOnLevel = 1;
            }
            if(MinimalOnLevel>Byte.MaxValue)
            {
                MinimalOnLevel = Byte.MaxValue;
            }
            if (InitialStoredTargetLevel<MinimalOnLevel || InitialStoredTargetLevel>Byte.MaxValue)
            {
                InitialStoredTargetLevel = Byte.MaxValue;
            }
            AutoOffIntervalMsecs = Math.Max(0, AutoOffIntervalMsecs);
            this.OutputResources=OutputResources;
            this.MinimalOnLevel=MinimalOnLevel;
            this.InitialStoredTargetLevel=InitialStoredTargetLevel;
            this.AutoOffIntervalMsecs=AutoOffIntervalMsecs;
            this.StandbyController=StandbyController;
        }
        public ISet<ushort> OutputResources{get;}
        public int MinimalOnLevel{get;}
        public int InitialStoredTargetLevel{get;}
        public uint AutoOffIntervalMsecs{get;}
        public ushort StandbyController{get;}
        


        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            foreach(ushort pwm in OutputResources)
            {
                if (usedOutputPins.Contains(pwm.ToString()))
                {
                    return pwm.ToString();
                }
            }
            foreach (ushort pwm in OutputResources)
            {
                usedOutputPins.Add(pwm.ToString());
            }
            return string.Empty;
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

        public override string GenerateInitializer(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AFL("// cSinglePWM {0} (Dimmer for one color)", ApplicationName);
            sb.AppendFormat("sensact::apps::cSinglePWM {0}(eApplicationID::{0}, {1}, {2}, {3}, {4}, eApplicationID::{5});" + Environment.NewLine + Environment.NewLine, ApplicationName, VectorOfInOutIds(this.OutputResources, m), MinimalOnLevel, InitialStoredTargetLevel, AutoOffIntervalMsecs, m.GetNameFromId(StandbyController));
            return sb.ToString();
        }

        public override string GenerateHTMLUserInterface(ModelContainerForCodeGenerator m)
        {
            StringBuilder sb = new StringBuilder();
            //sb.AppendFormat("<!--SinglePWM {0}--> "+Environment.NewLine, ApplicationDescription??ApplicationName);
            //sb.AppendFormat("<div class='app'><div><h2>{1}</h2><p>{2}</p></div><div><input type='range' min='1' max='100' value='50' onchange='MyApp.singlepwm_slider(this, {0})'><input onchange='MyApp.singlepwm_toggle(this, {0})' class='toggle' type='checkbox' /></div></div>"+Environment.NewLine+Environment.NewLine, ApplicationId, ApplicationName, ApplicationDescription );
            sb.AppendFormat("//SinglePWM {0} "+Environment.NewLine, ApplicationDescription??ApplicationName);
            sb.AppendFormat("ret.push(new SinglePwmApplication(ApplicationId.ApplicationId_{1}, '{1}', '{2}'));"+Environment.NewLine+Environment.NewLine, ApplicationId, ApplicationName, ApplicationDescription );
            return sb.ToString();
        }

        //PWM PushButton
        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("PWM__");
            }
        }
    }
}
