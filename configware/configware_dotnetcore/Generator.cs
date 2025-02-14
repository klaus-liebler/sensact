using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

using Klli.Sensact.Model.Common;
using Klli.Sensact.Model.Common.Applications;
using System.Linq;
using System.Text;
using Microsoft.Extensions.Logging;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config
{
    

    public enum TargetDirectory
    {
        COMMON,
        NODE_SPECIFIC,
    }

    public class SourceCodeGeneratorOptions
    {
        public const string SectionName = "SourceCodeGenerator";

        public string BasePath { get; set; }
        public int ANumber { get; set; }
    }
    public class SourceCodeGenerator
    {
        ILogger<SourceCodeGenerator> LOG;
        SourceCodeGeneratorOptions options;
       public SourceCodeGenerator(AppSettings appSettings, ILogger<SourceCodeGenerator> LOG)
        {
            this.options = appSettings.SourceCodeGenerator;
            this.LOG = LOG;
        }

        public void generateAll(ModelContainer mc){
            GenerateApplicationIds(mc);
            GenerateNodeIds(mc);
            GenerateApplicationNames(mc);
            
            GenerateParseCommand();
            
            GenerateCommandTypes(mc);
            GenerateCommandNames(mc);
            GenerateEventTypes(mc);
            GenerateEventNames(mc);
            GenerateEmptyImplementationForCmdHandler();
            GenerateCommandHandlerDeclarations(true);
            GenerateCommandHandlerDeclarations(false);
            
            GenerateSendCommandDeclarations(true);
            GenerateSendCommandDeclarations(false);
            GenerateSendCommandImplementation();
            
            GeneratePublishEventDeclarations(true);
            GeneratePublishEventDeclarations(false);
            GeneratePublishEventImplementation();

            GenerateHeaderIncludesForApplications(mc);
            GenerateNodeSpecificFiles(mc);

            GenerateJSONDescription(mc);
            GenerateTypescriptUserInterface(mc);
        }
        
        public bool CheckAndPrepare(ModelContainer mc)
        {
            HashSet<ushort> predefinedAppIds = Enum.GetValues<Model.ApplicationId>().Select(x=>(ushort)x).ToHashSet();
            mc.NextFreeIndex = (ushort)(1+predefinedAppIds.Where(x=>x!=(ushort)Model.ApplicationId.NO_APPLICATION).Max());

            SensactApplicationContainer masterApp = new SensactApplicationContainer(null, new MasterApplication((ushort)Model.ApplicationId.MASTER, Model.ApplicationId.MASTER.ToString()));
            mc.id2app[masterApp.Application.ApplicationId] = masterApp;
            Model.Common.Nodes.Node monitoring_node= new Model.Common.Nodes.Monitoring(ushort.MaxValue, "MONITORING");
            mc.Model.Nodes.Add(monitoring_node);
            
            foreach (Sensact.Model.Common.Nodes.Node n in mc.Model.Nodes)
            {  
                HashSet<string> usedInputPins = new HashSet<string>();
                HashSet<string> usedOutputPins = new HashSet<string>();
                
                foreach (SensactApplication app in n.Applications)
                {
                    if(app.ApplicationName==null){
                        LOG.LogError("An App in Node {0} has no name", n.NodeName);
                        return false;
                    }
                    if(app.ApplicationId==0 && app.ApplicationName!=null){
                        app.SetApplicationId_BeCareful(mc.NextFreeIndex++);
                    }
                    
                    if (mc.id2app.ContainsKey(app.ApplicationId))
                    {
                        LOG.LogError("AppId {0} with name {1} is defined at least two times in node applications", app.ApplicationId, app.ApplicationName);
                        return false;
                    }
                    if (!app.HasValidAppId())
                    {
                        LOG.LogWarning("AppId {0} with name {1} in node {2} does not fulfill the regex pattern {3} for application name. This is ok, but maybe it is an error...", app.ApplicationId, app.ApplicationName, n.NodeName, app.AppNameRegex);
                        app.HasValidAppId();
                    }
                    string errorMessage = app.CheckAndAddUsedPins(usedInputPins, usedOutputPins);
                    if (errorMessage != string.Empty)
                    {
                        LOG.LogError("AppId {0} uses pins that have been reserved by another app:\n{1}", app.ApplicationId, errorMessage);
                        return false;
                    }
                    SensactApplicationContainer cont = new SensactApplicationContainer(n, app);
                    mc.id2app[cont.Application.ApplicationId] = cont;
                }

                //Sanity: Check, whether node application has already been added
                if(mc.id2app.ContainsKey(n.NodeId)){
                    //Es wurde eine Application mit der ID der Node angelegt
                    if(!mc.id2app[n.NodeId].Application.ApplicationName.Equals(n.NodeName)){
                        //mit dem Namen gibt es ein Problem
                        LOG.LogError("There is a predefined node app for node {0}, but its name is not correct:", n.NodeName);
                        return false;
                    }
                }else if(n!=monitoring_node){//add a special node app if it is not the monitoring node
                    SensactApplicationContainer nodeAppContainer = new SensactApplicationContainer(n, new SensactNodeApplication(n.NodeId, n.NodeName));
                    n.Applications.Add(nodeAppContainer.Application);
                    mc.id2app.Add(nodeAppContainer.Application.ApplicationId, nodeAppContainer);
                }                
            }

            //Find out which events should be fired by each application because there is a listener for the event
            //distinguish between local events and bus events
            foreach (Sensact.Model.Common.Nodes.Node n in mc.Model.Nodes)
            {
                foreach (SensactApplication app in n.Applications)
                {
                    HashSet<Event> evts = app.IReactOnTheseEvents();
                    SensactApplicationContainer appCont = mc.id2app[app.ApplicationId];
                    foreach (Event evt in evts)
                    {
                        //Kann die SourceApp dieses Event überhaupt erzeugen?
                        SensactApplicationContainer source;
                        if (!mc.id2app.TryGetValue(evt.SourceAppId, out source))
                        {
                            LOG.LogError("AppId {0} listens to event from AppId {1}, but this app does not exist.", app.ApplicationId, evt.SourceAppId);
                            return false;
                        }
                        else
                        {
                            if (!source.Application.ICanSendTheseEvents().Contains(evt.EventType))
                            {
                                LOG.LogError("AppId {0} listens to event {1} from AppId {2}, but this app cannot produce such events.", app.ApplicationId, evt.EventType, evt.SourceAppId);
                                return false;
                            }
                        }
                        if (appCont.Node == source.Node)
                        {
                            //source und destination leben in der selben node
                            HashSet<EventType> set = null;
                            if (!mc.id2localEvents.TryGetValue(evt.SourceAppId, out set))
                            {
                                set = new HashSet<EventType>();
                                mc.id2localEvents[evt.SourceAppId] = set;
                            }
                            set.Add(evt.EventType);
                        }
                        else
                        {
                            HashSet<EventType> set = null;
                            if (!mc.id2busEvents.TryGetValue(evt.SourceAppId, out set))
                            {
                                set = new HashSet<EventType>();
                                mc.id2busEvents[evt.SourceAppId] = set;
                            }
                            set.Add(evt.EventType);
                        }
                    }
                    HashSet<Command> cmds = app.ISendTheseCommands();
                    foreach (Command cmd in cmds)
                    {
                        SensactApplicationContainer target;
                        if (!mc.id2app.TryGetValue(cmd.TargetAppId, out target))
                        {
                            if (cmd.TargetAppId != (ushort)Model.ApplicationId.NO_APPLICATION)
                            {
                                LOG.LogError("AppId {0} sends command to AppId {1}, but this app does not exist.", app.ApplicationId, cmd.TargetAppId);
                                return false;
                            }
                        }
                        else
                        {
                            if (!target.Application.ICanReactOnTheseCommands().Contains(cmd.CommandType))
                            {
                                LOG.LogError("AppId {0} sends command {1} to AppId {2}, but this app cannot react on this command.", app.ApplicationId, cmd.CommandType, cmd.TargetAppId);
                                return false;
                            }
                        }
                    }

                }
            }

            return true;
        }

        public string AssemblyDirectory
        {
            get
            {
                string codeBase = Assembly.GetExecutingAssembly().Location;
                UriBuilder uri = new UriBuilder(codeBase);
                string path = Uri.UnescapeDataString(uri.Path);
                return Path.GetDirectoryName(path);
            }
        }
        protected void GenerateApplicationIds(ModelContainer mc)
        {
            StringBuilder sb_c = new StringBuilder();
            StringBuilder sb_flatc = new StringBuilder();
        
            //mc.Model.Nodes.ForEach(n => page.Nodes.Add(n.Id));
            //page.Nodes.Add("CNT");
            ushort CNT=0;
            foreach (var id in Enum.GetValues<Model.ApplicationId>())
            {
                sb_c.AppendLine("\t"+id.ToString() + "=" + (ushort)id + ",");
                sb_flatc.AppendLine("\t"+id.ToString() + "=" + (ushort)id + ",");
                if(id!=Model.ApplicationId.NO_APPLICATION){
                    CNT=Math.Max(CNT, (ushort)id);
                }
            }
            sb_c.AF2L("CNT={0},", CNT+1);
            this.WriteCommonFile("applicationIds", sb_c);
            this.WriteCommonFile("applicationIds.fbs", sb_flatc);
            LOG.LogInformation("Successfully created appids");
            return;
        }

        private static Type[] GetTypesInNamespace(Assembly assembly, string nameSpace)
        {
            return assembly.GetTypes().Where(t => string.Equals(t.Namespace, nameSpace, StringComparison.Ordinal)).ToArray();
        }

        protected void GenerateParseCommand()
        {
            //case eCommandType::SET_SIGNAL: OnSET_SIGNALCommand(ParseUInt16(payload, 0), now); break;
            StringBuilder sb = new StringBuilder();
            foreach (MethodInfo mi in typeof(SensactApplication).GetMethods())
            {

                if (mi.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.AppendFormat("\tcase sensact::eCommandType::{0}: app->{1}(", SensactApplication.ExtractCmdName(mi), mi.Name);
                int offset = 0;
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.Append(CS2CPPParser(pi.ParameterType, ref offset));
                    sb.Append(", ");
                }
                sb.AppendLine("ctx); break;");
            }
            WriteCommonFile("parseCommand", sb);
            LOG.LogInformation("Successfully created parseCommand");
        }

        protected void GenerateSendCommandImplementation()
        {
            StringBuilder sb_c = new StringBuilder();
            StringBuilder sb_typescript = new StringBuilder();
            foreach (MethodInfo mi in typeof(SensactApplication).GetMethods())
            {

                if (mi.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb_c.AppendFormat("\tvoid cApplicationHost::Send{0}Command(sensact::eApplicationID destinationApp", SensactApplication.ExtractCmdName(mi));
                sb_typescript.AppendFormat("export function Send{0}Command(destinationApp:ApplicationId", SensactApplication.ExtractCmdName(mi));
                
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb_c.Append(", ");
                    sb_c.Append(CS2CPPType(pi.ParameterType));
                    sb_c.Append(" " + pi.Name);
                    sb_typescript.Append(", ");
                    sb_typescript.Append(pi.Name);
                    sb_typescript.Append(":"+CS2TSType(pi.ParameterType));
                    
                }
                sb_c.AppendLine(")");
                sb_typescript.AppendLine(", ctx:ISensactContext)");
                sb_c.AppendLine("\t{");
                sb_typescript.AppendLine("{");
                sb_c.AppendLine("\t\tuint8_t buffer[8];");
                
                int offset_c= 0;
                int offset_ts=0;
                StringBuilder sb_typescript_params = new StringBuilder();
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb_c.AppendLine("\t\t" + CS2CPPWriter(pi, ref offset_c));   
                    sb_typescript_params.AppendLine("\t" + CS2TSWriter(pi, ref offset_ts));
                }
                sb_c.AFL("\t\tthis->SendApplicationCommandToMessageBus(destinationApp, sensact::eCommandType::{0}, buffer, {1});", SensactApplication.ExtractCmdName(mi), offset_c);
                sb_c.AppendLine("\t}");
                
                sb_typescript.AFL("\tvar view = new DataView(new ArrayBuffer({0}));", offset_ts);
                sb_typescript.Append(sb_typescript_params.ToString());
                sb_typescript.AFL("\tctx.SendCommandMessage(destinationApp, Command.{0}, view);", SensactApplication.ExtractCmdName(mi), offset_ts);
                sb_typescript.AppendLine("}");
            }
            WriteCommonFile("sendCommandImplementation", sb_c);
            WriteCommonFile("sendCommandImplementation.ts", sb_typescript);
            
            LOG.LogInformation("Successfully created sendCommandImplementation for C++ and Typescript");
        }

        protected void GeneratePublishEventImplementation()
        {
            StringBuilder sb = new StringBuilder();
            foreach (EventType e in Enum.GetValues(typeof(EventType)))
            {
                sb.AppendFormat("\tvoid cApplicationHost::Publish{0}Event(sensact::eApplicationID sourceApp, const uint8_t *const payload, uint8_t payloadLength", e.ToString());
                sb.AppendLine(")");
                sb.AppendLine("\t{");
                sb.AppendFormat("\t\tthis->PublishApplicationEventToMessageBus(sourceApp, sensact::eEventType::{0}, payload, payloadLength);", e.ToString());
                sb.AppendLine();
                sb.AppendLine("\t}");
            }
            WriteCommonFile("publishEventImplementation", sb);
            LOG.LogInformation("Successfully created publishEventImplementation");
        }
        protected void GenerateEmptyImplementationForCmdHandler()
        {
            //case eCommandType::SET_SIGNAL: OnSET_SIGNALCommand(ParseUInt16(payload, 0), now); break;
            StringBuilder sb = new StringBuilder();
            foreach (MethodInfo mi in typeof(SensactApplication).GetMethods())
            {
                if (mi.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.AppendFormat("\tvoid cApplication::{0}(", mi.Name);
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.Append(CS2CPPType(pi.ParameterType));
                    sb.Append(" " + pi.Name + ", ");
                }
                sb.AppendLine("SensactContext *ctx)");
                sb.AppendLine("\t{");
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.AppendLine("\t\t(void)(" + pi.Name + ");");
                }
                sb.AppendLine("\t\t(void)(ctx);");
                sb.AppendLine("\t\tLOGE(\"Application %s does not support Command " + SensactApplication.ExtractCmdName(mi) + "\", N());");
                sb.AppendLine("\t}");
                sb.AppendLine();
            }
            WriteCommonFile("cmdHandler", sb);
        }

        private void GenerateCommandHandlerDeclarations(bool TrueIfVirtualFalseIfOverride)
        {
            StringBuilder sb = new StringBuilder();
            //void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, SensactContext *ctx) override;
            foreach (MethodInfo m in typeof(SensactApplication).GetMethods())
            {

                if (m.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.Append("\t");
                if(TrueIfVirtualFalseIfOverride){
                    sb.Append("virtual ");
                }
                sb.Append("void " + m.Name + "(");
                foreach (ParameterInfo pi in m.GetParameters())
                {
                    sb.Append(CS2CPPType(pi.ParameterType));
                    sb.Append(" " + pi.Name + ", ");
                }
                sb.Append("iSensactContext *ctx)");
                 if(TrueIfVirtualFalseIfOverride){
                    sb.AppendLine("{return;}");
                }else{
                    sb.AppendLine(" override;");
                }
            }
            if(TrueIfVirtualFalseIfOverride){
                WriteCommonFile("commandHandlerDeclarationsVirtual", sb);
            }else{
                WriteCommonFile("commandHandlerDeclarationsOverride", sb);
            }
        }

        private void GeneratePublishEventDeclarations(bool TrueIfVirtualFalseIfOverride)
        {
            StringBuilder sb = new StringBuilder();
            foreach(EventType e in Enum.GetValues(typeof(EventType))){
                sb.Append("\t");
                if(TrueIfVirtualFalseIfOverride){
                    sb.Append("virtual ");
                }
                sb.AppendFormat("void Publish{0}Event(sensact::eApplicationID sourceApp, const uint8_t *const payload, uint8_t payloadLength", e.ToString());
                if(TrueIfVirtualFalseIfOverride){
                    sb.AppendLine(")=0;");
                }else{
                    sb.AppendLine(") override;");
                }
                
            }

            if(TrueIfVirtualFalseIfOverride){
                WriteCommonFile("publishEventDeclarationsVirtual", sb);
            }else{
                WriteCommonFile("publishEventDeclarationsOverride", sb);
            }
        }

        private void GenerateSendCommandDeclarations(bool TrueIfVirtualFalseIfOverride)
        {
            StringBuilder sb = new StringBuilder();
            //void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, SensactContext *ctx) override;
            foreach (MethodInfo m in typeof(SensactApplication).GetMethods())
            {
                if (m.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.Append("\t");
                if(TrueIfVirtualFalseIfOverride){
                    sb.Append("virtual ");
                }
                sb.AppendFormat("void Send{0}Command(sensact::eApplicationID destinationApp", SensactApplication.ExtractCmdName(m));
               
                foreach (ParameterInfo pi in m.GetParameters())
                {
                    sb.Append(", ");
                    sb.Append(CS2CPPType(pi.ParameterType));
                    sb.Append(" " + pi.Name);
                }
                if(TrueIfVirtualFalseIfOverride){
                    sb.AppendLine(")=0;");
                }else{
                    sb.AppendLine(") override;");
                }
                
            }

            if(TrueIfVirtualFalseIfOverride){
                WriteCommonFile("sendCommandDeclarationsVirtual", sb);
            }else{
                WriteCommonFile("sendCommandDeclarationsOverride", sb);
            }
            
        }

        private StringBuilder HeaderForCommandsOfType(Type t)
        {
            StringBuilder sb = new StringBuilder();
            //void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, SensactContext *ctx) override;
            foreach (MethodInfo m in t.GetMethods())
            {

                if (m.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                if (m.IsOverride())
                {
                    sb.Append("\tvoid " + m.Name + "(");
                    foreach (ParameterInfo pi in m.GetParameters())
                    {
                        sb.Append(CS2CPPType(pi.ParameterType));
                        sb.Append(" " + pi.Name + ", ");
                    }
                    sb.AppendLine("iSensactContext *ctx) override;");

                }
            }
            return sb;
        }

        internal void GenerateHeaderIncludesForApplications(ModelContainer mc)
        {
            Type[] applications = GetTypesInNamespace(Assembly.GetExecutingAssembly(), "Klli.Sensact.Config.Applications");
            foreach (Type t in applications)
            {
                string filename = t.Name;
                this.WriteFileInSubdirectory("application", filename, HeaderForCommandsOfType(t));
            }
        }

        private static string CS2CPPType(Type t)
        {
            if (t == typeof(int)) return "int32_t";
            else if (t == typeof(uint)) return "uint32_t";
            else if (t == typeof(short)) return "int16_t";
            else if (t == typeof(ushort)) return "uint16_t";
            else if (t == typeof(sbyte)) return "int8_t";
            else if (t == typeof(byte)) return "uint8_t";
            else throw new ArgumentException("Type " + t + " is unknown");
        }

        private static string CS2TSType(Type t)
        {
            if (t == typeof(int)) return "number";
            else if (t == typeof(uint)) return "number";
            else if (t == typeof(short)) return "number";
            else if (t == typeof(ushort)) return "number";
            else if (t == typeof(sbyte)) return "number";
            else if (t == typeof(byte)) return "number";
            else throw new ArgumentException("Type " + t + " is unknown");
        }

        private string CS2CPPParser(Type t, ref int offset)
        {
            string ret;
            if (t == typeof(int))
            {
                ret = "ParseI32(payload, " + offset + ")";
                offset += 4;
            }
            else if (t == typeof(uint))
            {
                ret = "ParseU32(payload, " + offset + ")";
                offset += 4;
            }
            else if (t == typeof(short))
            {
                ret = "ParseI16(payload, " + offset + ")";
                offset += 2;
            }
            else if (t == typeof(ushort))
            {
                ret = "ParseU16(payload, " + offset + ")";
                offset += 2;
            }
            else if (t == typeof(sbyte))
            {
                ret = "(int8_t)payload[" + offset + "]";
                offset += 1;
            }
            else if (t == typeof(byte))
            {
                {
                    ret = "payload[" + offset + "]";
                    offset += 1;
                }
            }
            else throw new ArgumentException("Type " + t + " is unknown");
            return ret;
        }

        private string CS2CPPWriter(ParameterInfo pi, ref int offset)
        {
            string ret;
            if (pi.ParameterType == typeof(int))
            {
                ret = "WriteI32(" + pi.Name + ", buffer, " + offset + ");";
                offset += 4;
            }
            else if (pi.ParameterType == typeof(uint))
            {
                ret = "WriteU32(" + pi.Name + ", buffer, " + offset + ");";
                offset += 4;
            }
            else if (pi.ParameterType == typeof(short))
            {
                ret = "WriteI16(" + pi.Name + ", buffer, " + offset + ");";
                offset += 2;
            }
            else if (pi.ParameterType == typeof(ushort))
            {
                ret = "WriteU16(" + pi.Name + ", buffer, " + offset + ");";
                offset += 2;
            }
            else if (pi.ParameterType == typeof(sbyte))
            {
                ret = "buffer[" + offset + "]=(int8_t)" + pi.Name + ";";
                offset += 1;
            }
            else if (pi.ParameterType == typeof(byte))
            {
                {
                    ret = "buffer[" + offset + "]=" + pi.Name + ";";
                    offset += 1;
                }
            }
            else throw new ArgumentException("Type " + pi.ParameterType + " is unknown");
            return ret;
        }

        private string CS2TSWriter(ParameterInfo pi, ref int offset)
        {
            string ret;
            if (pi.ParameterType == typeof(uint))
            {
                ret = "view.setUint32(" + offset + "," + pi.Name+ ");";
                offset += 4;
            }
            else if (pi.ParameterType == typeof(short))
            {
                ret = "view.setInt16(" + offset + "," + pi.Name+ ");";
                offset += 2;
            }
            else if (pi.ParameterType == typeof(ushort))
            {
                ret = "view.setUint16(" + offset + "," + pi.Name+ ");";
                offset += 2;
            }
            else if (pi.ParameterType == typeof(sbyte))
            {
                ret = "view.setUint8(" + offset + "," + pi.Name+ ");";
                offset += 1;
            }
            else if (pi.ParameterType == typeof(byte))
            {
                
                ret = "view.setInt8(" + offset + "," + pi.Name+ ");";
                offset += 1;
                
            }
            else throw new ArgumentException("Type " + pi.ParameterType + " is unknown");
            return ret;
        }

        internal void GenerateNodeIds(ModelContainer mc){
            StringBuilder sb = new StringBuilder();
            foreach (Sensact.Model.Common.Nodes.Node n in mc.Model.Nodes)
            {

                sb.AppendLine(n.NodeName+",");
            }
            WriteCommonFile("nodeIds", sb);
        }
        internal void GenerateApplicationNames(ModelContainer mc)
        {
            var ApplicationNames = new string[mc.NextFreeIndex];
            foreach (var kv in Enum.GetValues<Model.ApplicationId>().Where(x=>x!=Model.ApplicationId.NO_APPLICATION))
            {
                ApplicationNames[(ushort)kv] = kv.ToString();
                //Alle vordefinierten Indizes müssen bekannt sein, auch wenn die App (noch) nicht definiert ist
            }
            foreach(var appc in mc.id2app.Values){
                //Sanity check
                try
                {
                    if(ApplicationNames[appc.Application.ApplicationId]!=null && !ApplicationNames[appc.Application.ApplicationId].Equals(appc.Application.ApplicationName)){
                    throw new Exception("Implementation error!");
                }
                }
                catch (System.Exception)
                {
                    
                    throw;
                }
                
                ApplicationNames[appc.Application.ApplicationId]=appc.Application.ApplicationName;
                //Auch alle Apps, die keinen vordefinierten Index verwenden, müssen bekannt sein
            }

            StringBuilder sb = new StringBuilder();
            foreach (var name in ApplicationNames)
            {
                sb.AFL("\"{0}\",", name);
            }
            this.WriteCommonFile("applicationNames", sb);
        }

        internal void GenerateCommandNames(ModelContainer mc)
        {
            var CommandNames = new string[Enum.GetValues<CommandType>().Count()];
            foreach (var kv in Enum.GetValues<CommandType>())
            {
                CommandNames[(ushort)kv] = kv.ToString();
            }

            StringBuilder sb = new StringBuilder();
            foreach (var name in CommandNames)
            {
                sb.AFL("\"{0}\",", name);
            }
            this.WriteCommonFile("commandNames", sb);
        }

        internal void GenerateEventNames(ModelContainer mc)
        {
            var EventNames = new string[Enum.GetValues<EventType>().Count()];
            foreach (var kv in Enum.GetValues<EventType>())
            {
                EventNames[(ushort)kv] = kv.ToString();
            }

            StringBuilder sb = new StringBuilder();
            foreach (var name in EventNames)
            {
                sb.AFL("\"{0}\",", name);
            }
            this.WriteCommonFile("eventNames", sb);
        }

        internal void GenerateNodeSpecificFiles(ModelContainer mc)
        {
            
            StringBuilder sb = new StringBuilder();
            foreach (Sensact.Model.Common.Nodes.Node node in mc.Model.Nodes)
            {
                LOG.LogInformation("Create Files for node {0}", node.NodeName);
                //const eApplicationID MODEL::NodeMasterApplication = eApplicationID::SNSCT_L0_TECH_HS_1;
                if(node.NodeHasANodeApplication){
                    sb.AFL("const sensact::eApplicationID cApplications::NodeMasterApplication = sensact::eApplicationID::{0};", node.NodeName);
                }else{//2023-02-15: the only use case for this: The "MONITORING" node has no application
                    sb.AFL("const sensact::eApplicationID cApplications::NodeMasterApplication = sensact::eApplicationID::NO_APPLICATION;", node.NodeName);
                }
                WriteNodeSpecificFile(node, "nodeMasterApplicationId", sb);
                sb.Clear();

                //const char MODEL::ModelString[] ="NodeId SNSCT_L0_TECH_HS_1 created on 02.02.2021 22:29:40 using model Sattlerstraße 16 from git hash ea29f6371a5d33c7621cecf1e6bda050edf38681";
                sb.AFL("const char* const node::NodeDescription =\"NodeId {0} created on {1} using model {2}\";", node.NodeName, DateTime.Now, mc.Model.Name);
                //const eNodeID MODEL::NodeID = eNodeID::SNSCT_L0_TECH_HS_1;
                sb.AFL("const sensact::eNodeID node::NodeID = sensact::eNodeID::{0};", node.NodeName);
                sb.AFL("const char* const node::NodeName =\"{0}\";", node.NodeName);
                WriteNodeSpecificFile(node, "nodeDescription", sb);
                sb.Clear();

                string[] Glo2LocCmd = new string[mc.NextFreeIndex];
                for (int i = 0; i < mc.NextFreeIndex; i++)
                {
                    Glo2LocCmd[i] = "\tnullptr,";
                }
                //Static initializers
                foreach (SensactApplication app in node.Applications)
                {
                    sb.AFL("{0}",app.GenerateCommentAboveConstructor(mc));
                    sb.AF2L("{0};",app.GenerateCPPConstructor(mc));
                    SensactApplicationContainer appCont = mc.id2app[app.ApplicationId];
                    Glo2LocCmd[appCont.Application.ApplicationId] = "\t&" + app.ApplicationName + ",";
                }
                WriteNodeSpecificFile(node, "applicationInitializers", sb);
                sb.Clear();
                for (int i = 0; i < Glo2LocCmd.Length; i++)
                {
                    sb.AppendLine(Glo2LocCmd[i]);
                }
                WriteNodeSpecificFile(node, "glo2LocCmd", sb);
                sb.Clear();
                string[] Glo2LocEvents = new string[mc.NextFreeIndex];
                for (int i = 0; i < Glo2LocEvents.Length; i++)
                {
                    Glo2LocEvents[i] = "\tnullptr,";
                }
                for (int i = 0; i < Glo2LocCmd.Length; i++)
                {
                    sb.AppendLine(Glo2LocEvents[i]);
                }

                WriteNodeSpecificFile(node, "glo2LocEvt", sb);
                sb.Clear();
                //Typescript Constructor
                foreach (SensactApplicationContainer appc in mc.id2app.Values)
                {
                    if(appc.Node==null){
                        continue; //for MASTER Application
                    }
                    sb.AFL("{0}", appc.Application.GenerateCommentAboveConstructor(mc));
                    string constr=appc.Application.GenerateTypescriptConstructor(mc);
                    if(string.IsNullOrEmpty(constr)){
                        sb.AppendLine();
                        continue;
                    }
                    bool isLocal=appc.Node.NodeName.Equals(node.NodeName);

                    sb.AF2L("ret.push({{local:{0}, app:{1}}})", isLocal?"true":"false", constr );
                    
                }
                WriteNodeSpecificFile(node, "sensactapps_local.ts", sb);
                sb.Clear();
            }
            LOG.LogInformation("Successfully created all node specific .inc files");
            return;
        }

        private void GenerateJSONDescription(ModelContainer mc){
            
            foreach (Sensact.Model.Common.Nodes.Node node in mc.Model.Nodes)
            {
                string directory = Path.Combine(this.options.BasePath, node.NodeName);
                Directory.CreateDirectory(directory);

                var info = new Dictionary<string, string>()
                {
                    { "SENSACT_NODE_NAME", node.NodeName},
                    { "SENSACT_MODEL_NAME", mc.Model.Name},
                    { "SENSACT_MODEL_CREATION_DT", DateTimeOffset.Now.ToUnixTimeSeconds().ToString()},
                    { "SENSACT_MODEL_CREATION_DT_STRING", DateTime.Now.ToString()},
                };

                File.WriteAllText(
                    Path.Combine(directory, "node_descriptor.json"),
                    JsonSerializer.Serialize<object>(info)
                );

                List<object> list = new List<object>();
                foreach(var app in node.Applications){
                    app.AddJSONDescriptionToList(list);
                }
                
                
                string path = Path.Combine(directory, "app_descriptor.json");
                File.WriteAllText(path, JsonSerializer.Serialize<object>(list));
            }
            LOG.LogInformation("Successfully created all node specific .json files in {0}.", this.options.BasePath);
        }

        private void GenerateTypescriptUserInterface(ModelContainer mc){
            StringBuilder sb = new StringBuilder();
            foreach (SensactApplicationContainer appc in mc.id2app.Values)
            {
                sb.AFL("{0}",appc.Application.GenerateCommentAboveConstructor(mc));
                sb.AF2L("{0}",appc.Application.GenerateTypescriptConstructor(mc));
            }
            WriteCommonFile("sensactapps.ts", sb);
            LOG.LogInformation("Successfully created typescript file with UI for all apps");
        }

        protected void WriteNodeSpecificFile(Model.Common.Nodes.Node node, string filenameWithoutExtension, StringBuilder content)
        {
            WriteFileInSubdirectory(node.NodeName, filenameWithoutExtension, content);
        }
        
        protected void WriteFileInSubdirectory(string subdir, string filenameWithoutExtension, StringBuilder content)
        {
            string directory = Path.Combine(this.options.BasePath, subdir);
            Directory.CreateDirectory(directory);
            string path = Path.Combine(directory, filenameWithoutExtension + ".inc");
            File.WriteAllText(path, content.ToString());
        }

        public void DeleteBaseDirectory()
        {
            
            try
            {
                foreach(var subdir in Directory.GetDirectories(this.options.BasePath)){
                    Directory.Delete(subdir, true);
                }
            }
            catch (Exception) { }
        }

        protected void WriteCommonFile(string filenameWithoutExtension, StringBuilder content)
        {
            string directory = Path.Combine(this.options.BasePath, "common");
            Directory.CreateDirectory(directory);
            string path = Path.Combine(directory, filenameWithoutExtension + ".inc");
            File.WriteAllText(path, content.ToString());
        }

        internal void GenerateCommandTypes(ModelContainer model)
        {
            StringBuilder sb_c = new StringBuilder();
            StringBuilder sb_protobuf = new StringBuilder();
            
            foreach (CommandType ct in Enum.GetValues(typeof(CommandType)))
            {
                sb_c.AppendLine(ct.ToString() + "=" + (int)ct + ",");
                sb_protobuf.AppendLine("\t"+ct.ToString() + "=" + (int)ct + ",");
            }
            WriteCommonFile("commandTypes", sb_c);
            WriteCommonFile("commandTypes.fbs", sb_protobuf);
            
        }
        
        internal void GenerateEventTypes(ModelContainer model)
        {
            StringBuilder sb_c = new StringBuilder();
            StringBuilder sb_protobuf = new StringBuilder();
            sb_protobuf.AppendLine("enum Event:byte {");
            foreach (EventType ct in Enum.GetValues(typeof(EventType)))
            {
                sb_c.AppendLine(ct.ToString() + "=" + (int)ct + ",");
                sb_protobuf.AppendLine("\t"+ct.ToString() + "=" + (int)ct + ",");
            }
            sb_protobuf.AppendLine("}");
            WriteCommonFile("eventTypes", sb_c);
            WriteCommonFile("eventTypes.fbs", sb_protobuf);

        }
    }
}
