using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml.Serialization;

namespace Klli.Sensact.Config
{
    public class SensactApplicationContainer
    {
        public Node Node;
        public int Index;
        public SensactApplication Application;

    }
    public abstract class SensactApplication
    {
        public const string REGEX_FLOOR_ROOM_SUFFIX = "_(L0|L1|L2|L3|LX|LS|XX)_(LVNG|KTCH|KID1|KID2|BATH|CORR|TECH|WORK|BEDR|WELL|STO1|PRTY|STRS|XXX)_.*";

        public string ApplicationId;
        

        [XmlIgnore]
        internal abstract Regex AppIdRegex { get; }



        public static SensactApplication MergeInHigherPrioVal(SensactApplication higherPrio, SensactApplication lowerPrio)
        {
            
            foreach(FieldInfo f in lowerPrio.GetType().GetFields())
            {
                object lowerPrioVal = f.GetValue(lowerPrio);
                if(f.GetValue(higherPrio)==null && lowerPrioVal!=null)
                {
                    f.SetValue(higherPrio, lowerPrioVal);
                }
            }
            return higherPrio;
        }

        internal bool HasValidAppId()
        {
            return this.AppIdRegex.IsMatch(this.ApplicationId.ToString());
        }

        public HashSet<Event> IReactOnTheseEvents()
        {
            HashSet<Event> ret = new HashSet<Event>();
            foreach (FieldInfo pi in this.GetType().GetRuntimeFields())
            {
                Type type = pi.FieldType;
                if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(List<>))
                {
                    Type itemType = type.GetGenericArguments()[0];
                    if (itemType == typeof(Event) && pi.Name.EndsWith("Events"))
                    {
                        var val = pi.GetValue(this);
                        if (val != null)
                        {
                            List<Event> cmds = val as List<Event>;
                            ret.UnionWith(cmds);
                        }
                    }
                }

            }
            return ret;
        }
        public virtual HashSet<CommandType> ICanReactOnTheseCommands()
        {
            return new HashSet<CommandType>();
        }
        public HashSet<Command> ISendTheseCommands()
        {
            HashSet<Command> ret = new HashSet<Command>();
            foreach (FieldInfo pi in this.GetType().GetRuntimeFields())
            {
                Type type = pi.FieldType;
                if (type.IsGenericType && type.GetGenericTypeDefinition()== typeof(List<>))
                {
                    Type itemType = type.GetGenericArguments()[0]; 
                    if (itemType == typeof(Command) && pi.Name.StartsWith("Command"))
                    {
                        var val = pi.GetValue(this);
                        if(val!=null)
                        {
                            List<Command> cmds = val as List<Command>;
                            ret.UnionWith(cmds);
                        }
                    }
                }

            }
            return ret;
        }

        protected string CommandInitializer(string collectionName, ICollection<Command> cmds, ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("Command {0}{1}_{2}", (cmds == null || cmds.Count == 0) ? "*" : "", ApplicationId, collectionName);
            if (cmds != null && cmds.Count > 0)
            {
                sb.Append("[" + cmds.Count + "]={");
                foreach (Command cmd in cmds)
                {
                    sb.Append("{eApplicationID::" + cmd.TargetAppId + ", eCommandType::" + cmd.CommandType + "},");
                }

                sb.Append("}");
            }
            else
            {
                sb.Append("=0");
            }
            sb.AppendLine(";");
            return sb.ToString();
        }

        protected string ResourcesInitializer(string collectionName, ICollection<PwmPin> cmds, ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("ePWMOutput {0}{1}_{2}", (cmds == null || cmds.Count == 0) ? "*" : "", ApplicationId, collectionName);
            if (cmds != null && cmds.Count > 0)
            {
                sb.Append("[" + cmds.Count + "]={");
                foreach (PwmPin cmd in cmds)
                {
                    sb.Append("ePWMOutput::" + Convert.ToString(cmd) + ",");
                }
                sb.Append("}");
            }
            else
            {
                sb.Append("=0");
            }
            sb.AppendLine(";");
            return sb.ToString();
        }

        protected string EventInitializer(string prefix, ICollection<EventType> evts, ModelContainer m)
        {
            //eEventType PUSHB_EG_WOZ_G0S0_BusEvent[2] = {eEventType::PRESSED, eEventType::RELEASED, eEventType::END_OF_EVENTS};
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("eEventType {0}{1}_{2}Events", (evts == null || evts.Count == 0) ? "*" : "", ApplicationId, prefix);
            if (evts != null && evts.Count > 0)
            {
                sb.Append("[" + evts.Count + "]={");
                bool first = true;
                foreach (EventType evt in evts)
                {
                    if (!first)
                    {
                        sb.AppendLine("," + Environment.NewLine);
                    }
                    sb.AppendFormat("fEvent::{0}", evt);
                    first = false;
                }
                sb.Append("}");

            }
            else
            {
                sb.Append("=0");
            }
            sb.AppendLine(";");
            return sb.ToString();
        }

        public abstract HashSet<EventType> ICanSendTheseEvents();

        public abstract string GenerateInitializer(ModelContainer m);
    }

    

    public class Command
    {
        public string TargetAppId;
        public CommandType CommandType; //TODO: Prüfung: Kann die DestinationApp auf dieses Event reagieren?
    }

    

    //Jede Application kann Commands bei internen Events versenden und auf externe Events reagieren

    public abstract class SensorApplication:SensactApplication
    {

    }

    public abstract class ActorApplication:SensactApplication
    {

    }



    

   

    

   

    
}
