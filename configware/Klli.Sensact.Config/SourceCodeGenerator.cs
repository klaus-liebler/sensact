using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using log4net;
using Klli.Sensact.Config.Nodes;
using Klli.Sensact.Config.Templates;
using System.Linq;
using System.Text;

namespace Klli.Sensact.Config
{
    public static class SourceCodeGenerator
    {
        static ILog LOG = LogManager.GetLogger(typeof(SourceCodeGenerator));

        

        public static bool CheckAndPrepare(ModelContainer mc)
        {
            HashSet<string> alreadyDefinedAppIds = new HashSet<string>();
      
            mc.NextFreeIndex = 1; //0 has specific meaning


            SensactApplicationContainer masterApp =  new SensactApplicationContainer() { Application = new Applications.MasterApplication(), Index = 0, Node = null };
            mc.id2app[masterApp.Application.ApplicationId] = masterApp;
            mc.index2app[0] = masterApp;
            foreach (Node n in mc.Model.Nodes)
            {
                foreach(SensactApplication app in n.Applications)
                {
                    if(alreadyDefinedAppIds.Contains(app.ApplicationId))
                    {
                        LOG.ErrorFormat("AppId {0} is defined at least two times in node applications", app.ApplicationId);
                        return false;
                    }
                    if(!app.HasValidAppId())
                    {
                        LOG.WarnFormat("AppId {0} does not fulfill the recommendations for application name. This is ok, but maybe it is an error...", app.ApplicationId, n.Id);
                        app.HasValidAppId();
                    }
                    alreadyDefinedAppIds.Add(app.ApplicationId);
                    SensactApplicationContainer cont = new SensactApplicationContainer
                    {
                        Application = app,
                        Index = mc.NextFreeIndex,
                        Node = n,
                    };
                    mc.NextFreeIndex++;

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
                            LOG.ErrorFormat("AppId {0} listens to event from AppId {1}, but this app does not exist.", app.ApplicationId, evt.SourceAppId);
                            return false;
                        }
                        else
                        {
                            if(!source.Application.ICanSendTheseEvents().Contains(evt.EventType))
                            {
                                LOG.ErrorFormat("AppId {0} listens to event {1} from AppId {2}, but this app cannot produce such events.", app.ApplicationId, evt.EventType, evt.SourceAppId);
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
                    foreach(Command cmd in cmds)
                    {
                        SensactApplicationContainer target;
                        if (!mc.id2app.TryGetValue(cmd.TargetAppId, out target))
                        {
                            if(cmd.TargetAppId != ID.NO_APPLICATION.ToString())
                            {
                                LOG.ErrorFormat("AppId {0} sends command to AppId {1}, but this app does not exist.", app.ApplicationId, cmd.TargetAppId);
                                return false;
                            }
                        }
                        else
                        {
                            if (!target.Application.ICanReactOnTheseCommands().Contains(cmd.CommandType))
                            {
                                LOG.ErrorFormat("AppId {0} sends command {1} to AppId {2}, but this app cannot react on this command.", app.ApplicationId, cmd.CommandType, cmd.TargetAppId);
                                return false;
                            }
                        }
                    }

                }
            }
            
            return true;
        }

        public static string AssemblyDirectory
        {
            get
            {
                string codeBase = Assembly.GetExecutingAssembly().CodeBase;
                UriBuilder uri = new UriBuilder(codeBase);
                string path = Uri.UnescapeDataString(uri.Path);
                return Path.GetDirectoryName(path);
            }
        }

        internal static void GenerateAppIds_h(ModelContainer mc)
        {
            HC_APPIDS_H page = new HC_APPIDS_H()
            {
                version="1.0"
            };
            mc.Model.Nodes.ForEach(n => page.Nodes.Add(n.Id));
            page.Nodes.Add("CNT");

            page.AppIds.Add("MASTER=0");
            for (int i = 1; i < mc.NextFreeIndex; i++)
            {
                SensactApplicationContainer appCont = mc.index2app[i];
                page.AppIds.Add(appCont.Application.ApplicationId+"="+appCont.Index);
            }
            page.AppIds.Add("CNT");
            page.AppIds.Add("NO_APPLICATION");
            String pageContent = page.TransformText();
            File.WriteAllText(GetGeneratedPathForFile("appids.h"), pageContent);
            LOG.InfoFormat("Successfully created appids.h");
            return;
        }

        private static Type[] GetTypesInNamespace(Assembly assembly, string nameSpace)
        {
            return assembly.GetTypes().Where(t => string.Equals(t.Namespace, nameSpace, StringComparison.Ordinal)).ToArray();
        }

        public static bool IsOverride(this MethodInfo m)
        {
            return m.GetBaseDefinition().DeclaringType != m.DeclaringType;
        }

        public static string ExtractCmdName(MethodInfo mi)
        {
            return mi.Name.Substring(2, mi.Name.IndexOf("Command") - 2);
        }

        private static string ImplementationForCmdParse()
        {
            //case eCommandType::SET_SIGNAL: OnSET_SIGNALCommand(ParseUInt16(payload, 0), now); break;
            StringBuilder sb = new StringBuilder();
            foreach (MethodInfo mi in typeof(SensactApplication).GetMethods())
            {

                if (mi.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.AppendFormat("\tcase eCommandType::{0}: {1}(", ExtractCmdName(mi), mi.Name);
                int offset = 0;
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.Append(CS2CPPParser(pi.ParameterType, ref offset));
                    sb.Append(", ");
                }
                sb.AppendLine("now); break;");
            }
            return sb.ToString();
        }

        private static string ImplementationForCmdSend()
        {
            StringBuilder sb = new StringBuilder();
            foreach (MethodInfo mi in typeof(SensactApplication).GetMethods())
            {

                if (mi.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.AppendFormat("\tbool cApplication::Send{0}Command(eApplicationID destinationApp, ", ExtractCmdName(mi));
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.Append(CS2CPPType(pi.ParameterType));
                    sb.Append(" " + pi.Name + ", ");
                }
                sb.AppendLine("Time_t now)");
                sb.AppendLine("\t{");
                sb.AppendLine("\t\tuint8_t buffer[8];");
                int offset = 0;
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.AppendLine("\t\t"+CS2CPPWriter(pi, ref offset));
                    
                }
                sb.AppendFormat("\t\treturn cMaster::SendCommandToMessageBus(now, destinationApp, eCommandType::{0}, buffer, {1});", ExtractCmdName(mi), offset);
                sb.AppendLine();
                sb.AppendLine("\t}");
            }
            return sb.ToString();
        }
        private static string EmptyImplementationForCmdHandler()
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
                    sb.Append(" "+pi.Name+", ");
                }
                sb.AppendLine("Time_t now)");
                sb.AppendLine("\t{");
                foreach (ParameterInfo pi in mi.GetParameters())
                {
                    sb.AppendLine("\t\t(void)("+pi.Name+");");
                }
                sb.AppendLine("\t\t(void)(now);");
                sb.AppendLine("\t\tLOGE(\"Application %s does not support Command " + ExtractCmdName(mi) + "\", Name);");
                sb.AppendLine("\t}");
                sb.AppendLine();
            }
            return sb.ToString();
        }

        

        //	virtual void OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, Time_t now);
        //static bool CreateSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, uint8_t buffer, uint8_t* lenght);

        private static string HeaderForCommands()
        {
            StringBuilder sb = new StringBuilder();
            //void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
            foreach (MethodInfo m in typeof(SensactApplication).GetMethods())
            {

                if (m.GetCustomAttribute<SensactCommandMethod>() == null)
                {
                    continue;
                }
                sb.Append("\tvirtual void " + m.Name + "(");
                foreach (ParameterInfo pi in m.GetParameters())
                {
                    sb.Append(CS2CPPType(pi.ParameterType));
                    sb.Append(" " + pi.Name + ", ");
                }
                sb.Append("Time_t now)");
                sb.AppendLine(";");
                sb.AppendFormat("\tstatic bool Send{0}Command(eApplicationID destinationApp, ", ExtractCmdName(m));
                foreach (ParameterInfo pi in m.GetParameters())
                {
                    sb.Append(CS2CPPType(pi.ParameterType));
                    sb.Append(" " + pi.Name + ", ");
                }
                sb.AppendLine("Time_t now);");
                sb.AppendLine();
            }
            return sb.ToString();
        }

        private static string HeaderForCommandsOfType(Type t)
        {
            StringBuilder sb = new StringBuilder();
            //void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
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
                    sb.AppendLine("Time_t now) override;");
                   
                }
            }
            return sb.ToString();
        }

        internal static void GenerateHeaderIncludesForApplications(ModelContainer mc)
        {
            Type[] applications = GetTypesInNamespace(Assembly.GetExecutingAssembly(), "Klli.Sensact.Config.Applications");
            foreach (Type t in applications)
            {
                string filename = t.Name+".hinc";
                File.WriteAllText(GetGeneratedPathForFile(filename), HeaderForCommandsOfType(t));
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
            else throw new ArgumentException("Type "+t+" is unknown");
        }

        private static string CS2CPPParser(Type t, ref int offset)
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


        private static string CS2CPPWriter(ParameterInfo pi, ref int offset)
        {
            string ret;
            if (pi.ParameterType == typeof(int))
            {
                ret = "WriteInt32("+ pi.Name + ", buffer, " + offset + ");";
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
                    ret = "buffer[" + offset + "]="+ pi.Name + ";";
                    offset += 1;
                }
            }
            else throw new ArgumentException("Type " + pi.ParameterType + " is unknown");
            return ret;
        }


        internal static void GenerateModel_cpp(ModelContainer mc)
        {
            HC_MODEL_CPP file = new HC_MODEL_CPP()
            {
                version = "1.0",
                ModelReference = mc.Model.Name +" created from GIT Head SHA "+GeneratedConstants.ConfigSHA, 
            };
            for (int i = 0; i < mc.NextFreeIndex; i++)
            {
                SensactApplicationContainer app;
                if(mc.index2app.TryGetValue(i, out app))
                {
                    file.ApplicationNames.AppendLine("    \"" + app.Application.ApplicationId + "\",");
                }
                else
                {
                    file.ApplicationNames.AppendLine("    \"\",");
                }
                
            }
            foreach (Node node in mc.Model.Nodes)
            {
                ModelCppItem item = new ModelCppItem()
                {
                    version = "1.0",
                    NodeId = node.Id,
                    ModelInfo="NodeId "+node.Id+" created on "+DateTime.Now + " using model "+mc.Model.Name +" from git hash "+GeneratedConstants.ConfigSHA,
                };
                
                string[] Glo2LocPointers = new string[mc.NextFreeIndex];
                for (int i = 0; i < Glo2LocPointers.Length; i++)
                {
                    Glo2LocPointers[i] = "    0,";
                }
                //Static initializers
                foreach (SensactApplication app in node.Applications)
                {
                    item.StaticApplicationInitializers.Append(app.GenerateInitializer(mc));
                    SensactApplicationContainer appCont = mc.id2app[app.ApplicationId];
                    Glo2LocPointers[appCont.Index] = "    &" + app.ApplicationId+",";
                }
                for (int i = 0; i < Glo2LocPointers.Length; i++)
                {
                    item.Glo2LocPointers.AppendLine(Glo2LocPointers[i]);
                }
                file.Items.Add(item);
            }


            String pageContent = file.TransformText();
            string filename = "cModel.cpp";
            File.WriteAllText(GetGeneratedPathForFile(filename), pageContent);
            LOG.InfoFormat("Successfully created {0}", filename);
            return;
        }

        private static string GetGeneratedPathForFile(string filename)
        {
            return Path.Combine(Properties.Settings.Default.SensactDirectory, "firmware", "modules", "generated", filename);
        }

        internal static void GenerateCommandAndEventTypes_h(ModelContainer model)
        {
            HC_COMMANDANDEVENTTYPES_H page = new HC_COMMANDANDEVENTTYPES_H()
            {
                version = "1.0"
            };
            string pageContent = page.TransformText();
            File.WriteAllText(GetGeneratedPathForFile("commandAndEventTypes.h"), pageContent);
            LOG.InfoFormat("Successfully created commandAndEventTypes.h");
            return;
        }

        internal static void GenerateApplication_H_and_CPP(ModelContainer model)
        {
            APPLICATION_H h = new APPLICATION_H()
            {
                version = "1.0",
                CommandHeaders = HeaderForCommands(),
                
            };
            APPLICATION_CPP cpp = new APPLICATION_CPP()
            {
                version = "1.0",
                CommandParseImplementation = ImplementationForCmdParse(),
                CommandEmptyHandlerImplementation = EmptyImplementationForCmdHandler(),
                CommandCreateImplementation = ImplementationForCmdSend(),

            };

            string pageContent = h.TransformText();
            File.WriteAllText(GetGeneratedPathForFile("cApplication.h"), pageContent);
            LOG.InfoFormat("Successfully created cApplication.h");
            pageContent = cpp.TransformText();
            File.WriteAllText(GetGeneratedPathForFile("cApplication.cpp"), pageContent);
            LOG.InfoFormat("Successfully created cApplication.cpp");
            return;
        }
    }
}
