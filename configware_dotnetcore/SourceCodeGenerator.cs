using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

using Klli.Sensact.Config.Nodes;
using Klli.Sensact.Common;
using System.Linq;
using System.Text;
using Klli.Sensact.Config.Applications;
using Microsoft.Extensions.Logging;

namespace Klli.Sensact.Config
{
    public static class MyExtensions
    {
        public static StringBuilder AFL(this StringBuilder stringBuilder, string formatString, object obj)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine, obj);
        }

        public static StringBuilder AFL(this StringBuilder stringBuilder, string formatString, object o1, object o2, object o3)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine, o1, o2, o3);
        }

        public static StringBuilder AFL(this StringBuilder stringBuilder, string formatString, object o1, object o2)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine, o1, o2);
        }
    }

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

        public bool CheckAndPrepare(ModelContainer mc)
        {

            //preFill predefined indices
            mc.PrefillPredefinedIndices(Enum.GetValues(typeof(Klli.Sensact.Config.Nodes.ApplicationId)));

            HashSet<string> alreadyDefinedAppIds = new HashSet<string>();
            SensactApplicationContainer masterApp = new SensactApplicationContainer() { Application = new Applications.MasterApplication(), Index = mc.GetIndex("MASTER"), Node = null };
            mc.id2app[masterApp.Application.ApplicationId] = masterApp;
            mc.index2app[0] = masterApp;
            foreach (Node n in mc.Model.Nodes)
            {
                n.Applications.Add(new SensactNodeApplication(n.Id));
                HashSet<string> usedInputPins = new HashSet<string>();
                HashSet<string> usedOutputPins = new HashSet<string>();
                foreach (SensactApplication app in n.Applications)
                {
                    if (alreadyDefinedAppIds.Contains(app.ApplicationId))
                    {
                        LOG.LogError("AppId {0} is defined at least two times in node applications", app.ApplicationId);
                        return false;
                    }
                    if (!app.HasValidAppId())
                    {
                        LOG.LogWarning("AppId {0} does not fulfill the recommendations for application name. This is ok, but maybe it is an error...", app.ApplicationId, n.Id);
                        app.HasValidAppId();
                    }
                    string errorMessage = app.CheckAndAddUsedPins(usedInputPins, usedOutputPins);
                    if (errorMessage != null)
                    {
                        LOG.LogError("AppId {0} uses pins that have been reserved by another app:\n{1}", app.ApplicationId, errorMessage);
                        return false;
                    }
                    alreadyDefinedAppIds.Add(app.ApplicationId);
                    SensactApplicationContainer cont = new SensactApplicationContainer
                    {
                        Application = app,
                        Index = mc.GetIndex(app.ApplicationId),
                        Node = n,
                    };

                    mc.id2app[cont.Application.ApplicationId] = cont;
                    mc.index2app[cont.Index] = cont;
                }
            }

            //Find out which events should be fired by each application because there is a listener for the event
            //distinguish between local events and bus events
            foreach (Node n in mc.Model.Nodes)
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
                            if (cmd.TargetAppId != Nodes.ApplicationId.NO_APPLICATION.ToString())
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

        public void generateAll(ModelContainer mc){
            GenerateApplicationIds(mc);
            GenerateNodeIds(mc);
            GenerateApplicationNames(mc);
            GenerateParseCommand();
            
            GenerateCommandTypes(mc);
            GenerateEventTypes(mc);
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
        }
        protected void GenerateApplicationIds(ModelContainer mc)
        {
            StringBuilder pageContent = new StringBuilder();
            //mc.Model.Nodes.ForEach(n => page.Nodes.Add(n.Id));
            //page.Nodes.Add("CNT");

            foreach (var kv in mc.predefinedIndices)
            {
                pageContent.AppendLine(kv.Key + "=" + kv.Value + ",");
            }
            pageContent.AppendLine("CNT,");
            this.WriteCommonFile("applicationIds", pageContent);
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
            StringBuilder sb = new StringBuilder();
            foreach (MethodInfo mi in typeof(SensactApplication).GetMethods())
            {

                if (mi.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.AppendFormat("\tvoid cApplicationHost::Send{0}Command(sensact::eApplicationID destinationApp", SensactApplication.ExtractCmdName(mi));
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.Append(", ");
                    sb.Append(CS2CPPType(pi.ParameterType));
                    sb.Append(" " + pi.Name);
                }
                sb.AppendLine(")");
                sb.AppendLine("\t{");
                sb.AppendLine("\t\tuint8_t buffer[8];");
                int offset = 0;
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.AppendLine("\t\t" + CS2CPPWriter(pi, ref offset));

                }
                sb.AppendFormat("\t\tthis->SendApplicationCommandToMessageBus(destinationApp, sensact::eCommandType::{0}, buffer, {1});", SensactApplication.ExtractCmdName(mi), offset);
                sb.AppendLine();
                sb.AppendLine("\t}");
            }
            WriteCommonFile("sendCommandImplementation", sb);
            LOG.LogInformation("Successfully created sendCommandImplementation");
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
                sb.Append("SensactContext *ctx)");
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
                    sb.AppendLine("SensactContext *ctx) override;");

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

        private string CS2CPPParser(Type t, ref int offset)
        {
            string ret;
            if (t == typeof(int))
            {
                ret = "ParseInt32(payload, " + offset + ")";
                offset += 4;
            }
            else if (t == typeof(uint))
            {
                ret = "ParseUInt32(payload, " + offset + ")";
                offset += 4;
            }
            else if (t == typeof(short))
            {
                ret = "ParseInt16(payload, " + offset + ")";
                offset += 2;
            }
            else if (t == typeof(ushort))
            {
                ret = "ParseUInt16(payload, " + offset + ")";
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
                ret = "WriteInt32(" + pi.Name + ", buffer, " + offset + ");";
                offset += 4;
            }
            else if (pi.ParameterType == typeof(uint))
            {
                ret = "WriteUInt32(" + pi.Name + ", buffer, " + offset + ");";
                offset += 4;
            }
            else if (pi.ParameterType == typeof(short))
            {
                ret = "WriteInt16(" + pi.Name + ", buffer, " + offset + ");";
                offset += 2;
            }
            else if (pi.ParameterType == typeof(ushort))
            {
                ret = "WriteUInt16(" + pi.Name + ", buffer, " + offset + ");";
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

        internal void GenerateNodeIds(ModelContainer mc){
            StringBuilder sb = new StringBuilder();
            foreach (Node n in mc.Model.Nodes)
            {

                sb.AppendLine(n.Id+",");
            }
            WriteCommonFile("nodeIds", sb);
        }
        internal void GenerateApplicationNames(ModelContainer mc)
        {
            var ApplicationNames = new string[mc.NextFreeIndex];
            foreach (var kv in mc.predefinedIndices)
            {
                ApplicationNames[kv.Value] = kv.Key;
            }
            StringBuilder sb = new StringBuilder();
            foreach (var name in ApplicationNames)
            {
                sb.AFL("\"{0}\",", name);
            }
            this.WriteCommonFile("applicationNames", sb);
        }

        internal void GenerateNodeSpecificFiles(ModelContainer mc)
        {
            foreach (Node node in mc.Model.Nodes)
            {
                StringBuilder sb = new StringBuilder();

                //const eApplicationID MODEL::NodeMasterApplication = eApplicationID::SNSCT_L0_TECH_HS_1;
                sb.AFL("const sensact::eApplicationID applications::NodeMasterApplication = sensact::eApplicationID::{0};", node.Id);
                WriteFileInSubdirectory(node.Id, "nodeMasterApplicationId", sb);
                sb.Clear();

                //const char MODEL::ModelString[] ="NodeId SNSCT_L0_TECH_HS_1 created on 02.02.2021 22:29:40 using model Sattlerstraße 16 from git hash ea29f6371a5d33c7621cecf1e6bda050edf38681";
                sb.AFL("const char* const node::NodeDescription =\"NodeId {0} created on {1} using model {2}\";", node.Id, DateTime.Now, mc.Model.Name);
                //const eNodeID MODEL::NodeID = eNodeID::SNSCT_L0_TECH_HS_1;
                sb.AFL("const sensact::eNodeID node::NodeID = sensact::eNodeID::{0};", node.Id);
                WriteFileInSubdirectory(node.Id, "nodeDescription", sb);
                sb.Clear();

                string[] Glo2LocCmd = new string[mc.NextFreeIndex];
                for (int i = 0; i < mc.NextFreeIndex; i++)
                {
                    Glo2LocCmd[i] = "\tnullptr,";
                }
                //Static initializers
                foreach (SensactApplication app in node.Applications)
                {
                    sb.Append(app.GenerateInitializer(mc));
                    SensactApplicationContainer appCont = mc.id2app[app.ApplicationId];
                    Glo2LocCmd[appCont.Index] = "    &" + app.ApplicationId + ",";
                }
                WriteFileInSubdirectory(node.Id, "applicationInitializers", sb);
                sb.Clear();
                for (int i = 0; i < Glo2LocCmd.Length; i++)
                {
                    sb.AppendLine(Glo2LocCmd[i]);
                }
                WriteFileInSubdirectory(node.Id, "glo2LocCmd", sb);
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

                WriteFileInSubdirectory(node.Id, "glo2LocEvt", sb);
                sb.Clear();
            }
            LOG.LogInformation("Successfully created all node specific files");
            return;
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
                Directory.Delete(this.options.BasePath, true);
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
            StringBuilder stringBuilder = new StringBuilder();
            foreach (CommandType ct in Enum.GetValues(typeof(CommandType)))
            {

                stringBuilder.AppendLine(ct.ToString() + "=" + (int)ct + ",");
            }
            WriteCommonFile("commandTypes", stringBuilder);
        }
        internal void GenerateEventTypes(ModelContainer model)
        {
            StringBuilder stringBuilder = new StringBuilder();
            foreach (EventType ct in Enum.GetValues(typeof(EventType)))
            {

                stringBuilder.AppendLine(ct.ToString() + "=" + (int)ct + ",");
            }
            WriteCommonFile("eventTypes", stringBuilder);

        }
    }
}
