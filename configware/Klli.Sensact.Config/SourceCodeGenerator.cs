using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using log4net;
using Klli.Sensact.Config.Nodes;
using Klli.Sensact.Config.Templates;
using System.Linq;

namespace Klli.Sensact.Config
{
    public class SourceCodeGenerator
    {
        static ILog LOG = LogManager.GetLogger(typeof(SourceCodeGenerator));

        

        public static bool CheckAndPrepare(Model m)
        {
            Dictionary<string, SensactApplication> appId2rootApp = new Dictionary<string,SensactApplication>();

            foreach(SensactApplication app in m.Applications)
            {
                if(appId2rootApp.ContainsKey(app.ApplicationId))
                {
                    LOG.ErrorFormat("AppId {0} is defined at least two times in root applications", app.ApplicationId);
                    return false;
                }
                appId2rootApp[app.ApplicationId]=app;
            }
            
            HashSet<string> definedAppIds = new HashSet<string>();


            //Merge the Root-Part of Applications and the NodePart of Applications
            SensactApplication masterApp = new Applications.MasterApplication();
            m.Applications.Add(masterApp);
            int index = 1;
            foreach (Node n in m.Nodes)
            {
                foreach (SensactApplication app in n.Applications)
                {
                    app.Index = index;
                    index++;
                }
            }
            m.index2app = new SensactApplication[index];
            m.id2app[masterApp.ApplicationId] = masterApp;
            m.id2index[masterApp.ApplicationId] = masterApp.Index;
            m.index2app[masterApp.Index] = masterApp;
            foreach (Node n in m.Nodes)
            {
                foreach(SensactApplication app in n.Applications)
                {
                    app.Node = n;
                    if(definedAppIds.Contains(app.ApplicationId))
                    {
                        LOG.ErrorFormat("AppId {0} is defined at least two times in node applications", app.ApplicationId);
                        return false;
                    }
                    if(!app.HasValidAppId())
                    {
                        LOG.WarnFormat("AppId {0} does not fulfill the recommendations for application name. This is ok, but maybe it is an error...", app.ApplicationId, n.Id);
                    }
                    definedAppIds.Add(app.ApplicationId);
                    SensactApplication rootApp;
                    if(!appId2rootApp.TryGetValue(app.ApplicationId, out rootApp))
                    {
                        LOG.WarnFormat("AppId {0} exists in node {1}, but not in root applications. This is ok, but maybe it is an error...", app.ApplicationId, n.Id);
                    }else
                    {
                        SensactApplication.MergeInHigherPrioVal(app, rootApp);
                    }
                    m.id2app[app.ApplicationId] = app;
                    m.id2index[app.ApplicationId] = index;
                    m.index2app[app.Index] = app;
                }
                
            }

            //Find out which events should be fired by each application because there is a listener for the event
            //distinguish between local events and bus events
            foreach (Node n in m.Nodes)
            {
                foreach (SensactApplication app in n.Applications)
                {
                    HashSet<Event> evts = app.IReactOnTheseEvents();
                    foreach (Event evt in evts)
                    {
                        //Kann die SourceApp dieses Event überhaupt erzeugen?
                        SensactApplication source;
                        if(!m.id2app.TryGetValue(evt.SourceAppId, out source))
                        {
                            LOG.ErrorFormat("AppId {0} listens to event from AppId {1}, but this app does not exist.", app.ApplicationId, evt.SourceAppId);
                            return false;
                        }
                        else
                        {
                            if(!source.ICanSendTheseEvents().Contains(evt.EventType))
                            {
                                LOG.ErrorFormat("AppId {0} listens to event {1} from AppId {2}, but this app cannot produce such events.", app.ApplicationId, evt.EventType, evt.SourceAppId);
                                return false;
                            }
                        }
                        if (app.Node == m.id2app[evt.SourceAppId].Node)
                        {
                            //source und destination leben in der selben node
                            HashSet<EventType> set = null;
                            if (!m.id2localEvents.TryGetValue(evt.SourceAppId, out set))
                            {
                                set = new HashSet<EventType>();
                                m.id2localEvents[evt.SourceAppId] = set;
                            }
                            set.Add(evt.EventType);
                        }
                        else
                        {
                            HashSet<EventType> set = null;
                            if (!m.id2busEvents.TryGetValue(evt.SourceAppId, out set))
                            {
                                set = new HashSet<EventType>();
                                m.id2busEvents[evt.SourceAppId] = set;
                            }
                            set.Add(evt.EventType);
                        }
                    }
                    HashSet<Command> cmds = app.ISendTheseCommands();
                    foreach(Command cmd in cmds)
                    {
                        SensactApplication target;
                        if (!m.id2app.TryGetValue(cmd.TargetAppId, out target))
                        {
                            LOG.ErrorFormat("AppId {0} sends command to AppId {1}, but this app does not exist.", app.ApplicationId, cmd.TargetAppId);
                            return false;
                        }
                        else
                        {
                            if (!target.ICanReactOnTheseCommands().Contains(cmd.CommandType))
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

        internal static void GenerateAppIds_h(Model model)
        {
            HC_APPIDS_H page = new HC_APPIDS_H()
            {
                version="1.0"
            };
            model.Nodes.ForEach(n => page.Nodes.Add(n.Id));
            
            for(int i=0;i<model.index2app.Length;i++)
            {
                page.AppIds.Add(model.index2app[i].ApplicationId);
            }
            String pageContent = page.TransformText();
            File.WriteAllText(GetGeneratedPathForFile("appids.h"), pageContent);
            LOG.InfoFormat("Successfully created appids.h");
            return;
        }

        internal static void GenerateModel_cpp(Model m)
        {
            HC_MODEL_CPP file = new HC_MODEL_CPP()
            {
                version = "1.0",  
            };
            for (int i = 0; i < m.index2app.Length; i++)
            {
                SensactApplication app = m.index2app[i];
                file.ApplicationNames.AppendLine("    \"" + app.ApplicationId + "\",");
            }
            foreach (Node node in m.Nodes)
            {
                ModelCppItem item = new ModelCppItem()
                {
                    version = "1.0",
                    NodeId = node.Id,
                    ModelInfo="NodeId "+node.Id+" created on "+DateTime.Now,
                };
                
                string[] Glo2LocPointers = new string[m.id2index.Count];
                for (int i = 0; i < Glo2LocPointers.Length; i++)
                {
                    Glo2LocPointers[i] = "    0,";
                }
                //Static initializers
                foreach (SensactApplication app in node.Applications)
                {
                    item.StaticApplicationInitializers.Append(app.GenerateInitializer(m));
                    Glo2LocPointers[app.Index] = "    &" + app.ApplicationId+",";
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

        internal static void GenerateCommandAndEventTypes_h(Model model)
        {
            HC_COMMANDANDEVENTTYPES_H page = new HC_COMMANDANDEVENTTYPES_H()
            {
                version = "1.0"
            };
            foreach (string s in Enum.GetNames(typeof(EventType)))
            {
                page.EventTypes.Add(s);
            }
            foreach (string s in Enum.GetNames(typeof(CommandType)))
            {
                page.CommandTypes.Add(s);
            }
            String pageContent = page.TransformText();
            System.IO.File.WriteAllText(GetGeneratedPathForFile("commandAndEventTypes.h"), pageContent);
            LOG.InfoFormat("Successfully created commandAndEventTypes.h");
            return;
        }

        internal static void GenerateApplicationHandCPP(Model model)
        {
            APPLICATION_H h = new APPLICATION_H()
            {
                version = "1.0",
                Commands = (CommandType[])Enum.GetValues(typeof(CommandType)),
                
            };
            APPLICATION_CPP cpp = new APPLICATION_CPP()
            {
                version = "1.0",
                Commands = (CommandType[])Enum.GetValues(typeof(CommandType)),

            };
           
            String pageContent = h.TransformText();
            System.IO.File.WriteAllText(GetGeneratedPathForFile("cApplication.h"), pageContent);
            LOG.InfoFormat("Successfully created cApplication.h");
            pageContent = cpp.TransformText();
            System.IO.File.WriteAllText(GetGeneratedPathForFile("cApplication.cpp"), pageContent);
            LOG.InfoFormat("Successfully created cApplication.cpp");
            return;
        }
    }
}
