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

            page.AppIds.Add("MASTER");
            for (int i = 1; i < mc.NextFreeIndex; i++)
            {
                SensactApplicationContainer app = mc.index2app[i];
                page.AppIds.Add(app.Application.ApplicationId);
            }
            page.AppIds.Add("CNT");
            page.AppIds.Add("NO_APPLICATION");
            String pageContent = page.TransformText();
            File.WriteAllText(GetGeneratedPathForFile("appids.h"), pageContent);
            LOG.InfoFormat("Successfully created appids.h");
            return;
        }

        internal static void GenerateModel_cpp(ModelContainer mc)
        {
            HC_MODEL_CPP file = new HC_MODEL_CPP()
            {
                version = "1.0",  
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
                    ModelInfo="NodeId "+node.Id+" created on "+DateTime.Now,
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
            foreach (string s in Enum.GetNames(typeof(EventType)))
            {
                page.EventTypes.Add(s);
            }
            foreach (string s in Enum.GetNames(typeof(CommandType)))
            {
                page.CommandTypes.Add(s);
            }
            string pageContent = page.TransformText();
            File.WriteAllText(GetGeneratedPathForFile("commandAndEventTypes.h"), pageContent);
            LOG.InfoFormat("Successfully created commandAndEventTypes.h");
            return;
        }

        internal static void GenerateApplicationHandCPP(ModelContainer model)
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
