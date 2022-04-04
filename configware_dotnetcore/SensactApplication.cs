using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml.Serialization;
using Klli.Sensact.Common;

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
        private const string REGEX_FLOOR_ROOM_SUFFIX = "_(L0|L1|L2|L3|LX|LS|XX)_(LVNG|KTCH|KID1|KID2|BATH|CORR|TECH|WORK|BEDR|WELL|STO1|PRTY|STRS|UTIL|LEFT|RGHT|BACK|FRON|CARP|GARA|ROOF|XXX)_.*";

        public Regex FLOOR_ROOM_Regex(string prefix){
            return new Regex("^"+prefix+REGEX_FLOOR_ROOM_SUFFIX+"$");
        }
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

        internal abstract string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins);

        public HashSet<CommandType> ICanReactOnTheseCommands()
        {

            HashSet<CommandType> ret = new HashSet<CommandType>();
            foreach (MethodInfo m in GetType().GetMethods())
            {
                if (m.GetCustomAttribute<SensactCommandMethod>() == null)
                    continue;
                if (!m.IsOverride())
                    continue;
                ret.Add(MethodInfo2CommandType(m));
            }
            return ret;
        }

        public static string ExtractCmdName(MethodInfo mi)
        {
            return mi.Name.Substring(2, mi.Name.IndexOf("Command") - 2);
        }

        public static CommandType MethodInfo2CommandType(MethodInfo mi)
        {
            string cmdName = ExtractCmdName(mi);
            return (CommandType)Enum.Parse(typeof(CommandType), cmdName);
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

        //const uint8_t PUSHB_L1_LVNG_B31_OnPressed_payload1[3] = {1,2,3};

        private string BuildUint8Array(string name, byte[] payload)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("const uint8_t {0}[{1}] = {{", name, payload.Length);
            foreach (byte b in payload)
            {
                sb.Append(b + ",");
            }
            sb.Append("};"+Environment.NewLine);
            return sb.ToString();
        }

        protected string CommandInitializer(string collectionName, ICollection<Command> cmds, ModelContainer m)
        {
            StringBuilder payloads = new StringBuilder();
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("const Command {0}{1}_{2}", (cmds == null || cmds.Count == 0) ? "*const " : "", ApplicationId, collectionName);
            if (cmds != null && cmds.Count > 0)
            {
                sb.Append("[" + cmds.Count + "]={");
                int cmdIndex = 0;
                foreach (Command cmd in cmds)
                {
                    sb.Append("{eApplicationID::" + cmd.TargetAppId + ", eCommandType::" + cmd.CommandType + ", ");
                    if(cmd.Payload!=null && cmd.Payload.Length>0)
                    {
                        string nameOfArray = string.Format("{0}_{1}_{2}", ApplicationId, collectionName, cmdIndex);
                        payloads.Append(BuildUint8Array(nameOfArray, cmd.Payload));
                        sb.Append(nameOfArray+", " + cmd.Payload.Length + "}, ");
                    }
                    else
                    {
                        sb.Append("0, 0, },");
                    }
                    cmdIndex++;

                }

                sb.Append("}");
            }
            else
            {
                sb.Append("=0");
            }
            sb.AppendLine(";");

            payloads.Append(sb);
            return payloads.ToString();
        }

        protected string VectorOfInOutIds(ICollection<ushort> cmds, ModelContainer m)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("{");
            foreach (ushort cmd in cmds)
            {
                sb.Append(Convert.ToString(cmd) + ",");
            }
            sb.Append("}");
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

        [SensactCommandMethod]
        public virtual void OnNOPCommand() { }
        [SensactCommandMethod]
        public virtual void OnRESETCommand() { }
        [SensactCommandMethod]
        public virtual void OnSTART_IAPCommand() { }
        [SensactCommandMethod]
        public virtual void OnSTARTCommand() { }
        [SensactCommandMethod]
        public virtual void OnSTOPCommand() { }
        [SensactCommandMethod]
        public virtual void OnUPCommand(byte forced) { }
        [SensactCommandMethod]
        public virtual void OnDOWNCommand(byte forced) { }
        [SensactCommandMethod]
        public virtual void OnFORWARDCommand() { }
        [SensactCommandMethod]
        public virtual void OnBACKWARDCommand() { }
        [SensactCommandMethod]
        public virtual void OnLEFTCommand() { }
        [SensactCommandMethod]
        public virtual void OnRIGHTCommand() { }
        [SensactCommandMethod]
        public virtual void OnONCommand(uint autoReturnToOffMsecs) { }
        [SensactCommandMethod]
        public virtual void OnON_FILTERCommand(ushort targetApplicationId, uint autoReturnToOffMsecs) { }
        [SensactCommandMethod]
        public virtual void OnOFFCommand(uint autoReturnToOnMsecs) { }
        [SensactCommandMethod]
        public virtual void OnTOGGLECommand() { }
        [SensactCommandMethod]
        public virtual void OnTOGGLE_FILTERCommand(ushort targetApplicationId) { }
        [SensactCommandMethod]
        public virtual void OnTOGGLE_SPECIALCommand() { }
        [SensactCommandMethod]
        public virtual void OnSET_PARAMCommand() { }
        [SensactCommandMethod]
        public virtual void OnSET_HORIZONTAL_TARGETCommand(ushort target) { }
        [SensactCommandMethod]
        public virtual void OnSTEP_HORIZONTALCommand(short step) { }
        [SensactCommandMethod]
        public virtual void OnSET_VERTICAL_TARGETCommand(ushort target) { }
        [SensactCommandMethod]
        public virtual void OnSTEP_VERTICALCommand(short step) { }
        [SensactCommandMethod]
        public virtual void OnSET_LATERAL_TARGETCommand(ushort target) { }
        [SensactCommandMethod]
        public virtual void OnSTEP_LATERALCommand(short step) { }
        [SensactCommandMethod]
        public virtual void OnHEARTBEATCommand(uint sender) { }
        [SensactCommandMethod]
        public virtual void OnSEND_STATUSCommand() { }
        [SensactCommandMethod]
        public virtual void OnSET_RGBWCommand(byte R, byte G, byte B, byte W) { }
        /// <summary>
        /// Param is mp3 song id or well known color or light sequence)
        /// </summary>
        /// <param name="signal"></param>
        [SensactCommandMethod]
        public virtual void OnSET_SIGNALCommand(ushort signal) { }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="payload">just an ubiquitary payload that is used as payload for the PONG</param>
        [SensactCommandMethod]
        public virtual void OnPINGCommand(uint payload) { }
        /// <summary>
        /// Advises the sensact application to "play" a demo of its capabilities
        /// </summary>
        /// <param name="demostep"></param>
        [SensactCommandMethod]
        public virtual void OnDEMOCommand(uint demostep) { }

        [SensactCommandMethod]
        public virtual void OnSET_PWMCommand(uint bitmask, ushort value) { }

    }



    public class Command
    {
        public string TargetAppId;
        public CommandType CommandType; //TODO: Prüfung: Kann die DestinationApp auf dieses Event reagieren?
        public byte[] Payload;
    }


    public abstract class SensorApplication:SensactApplication
    {

    }

    public abstract class ActorApplication:SensactApplication
    {

    }

    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited =true)]
    public class SensactCommandMethod : Attribute
    {
        // . . .
    }
}
