using System.IO;
using log4net.Config;
using Klli.Sensact.Config.Nodes;
using System;
using log4net;

namespace KonCept.MMS.PDI.Plugin.PowerMacs4000API
{
    /// <summary>
    ///this class contains the Parameters of a Station (Object)
    ///in a PowerMACS System
    /// </summary>
    public class PowerMacsStation
    {
        private static readonly ILog LOG_STAT = LogManager.GetLogger(typeof(PowerMacsStation));
        private static readonly ILog LOG_CYCLIC = LogManager.GetLogger(typeof(PowerMacsStation).FullName + ".Cyclic");
        private static readonly ILog LOG_PROG = LogManager.GetLogger(typeof(Klli.Sensact.Config.Program));
    }
}

namespace Klli.Sensact.Config
{


    public class Program
    {
        private static readonly ILog LOG = LogManager.GetLogger(typeof(Program));
        private static readonly ILog LOG_TEST = LogManager.GetLogger(typeof(Program).FullName+".Test");
        private static Model GetModel()
        {
            //return TestModelBuilder.BuildRGBWDemo();
            //return TestModelBuilder.BuildModelToTestAllPins();
            //Model model = TestModelBuilder.BuildLIBARDemo();
            //return Sattlerstrasse16.Build();
            //return Sattlerstrasse16Classic.Build();
            return TestModelBuilder.BuildCANCommunicationDemo();
            //return TestModelBuilder.BuildBLINDDemo();
        }

        public static ModelContainer CreateAndCheckModelContainer()
        {
            Model model = GetModel();
            ModelContainer mc = new ModelContainer()
            {
                Model = model
            };
            if (SourceCodeGenerator.CheckAndPrepare(mc))
            {
                return mc;
            }
            return null;
        }

        static void Main(string[] args)
        {

            XmlConfigurator.Configure();
            ModelContainer mc = CreateAndCheckModelContainer();
            if (mc == null)
            {
                Console.WriteLine("See errors and then press any key to exit");
                Console.ReadKey();
                return;
            }
            SourceCodeGenerator.GenerateAppIds_h(mc);
            SourceCodeGenerator.GenerateModel_cpp(mc);
            SourceCodeGenerator.GenerateCommandAndEventTypes_h(mc);
            SourceCodeGenerator.GenerateApplication_H_and_CPP(mc);
            SourceCodeGenerator.GenerateHeaderIncludesForApplications(mc);

            if(Properties.Settings.Default.DoNotCompile)
            {
                return;
            }
            foreach (Node n in mc.Model.Nodes)
            {
                SensactFileCollector p = new SensactFileCollector();
                CompileOptions o = p.BuildCompileOptions(n);
                CompilerController cc = new CompilerController(o);
                string firmwareBinPath = Path.Combine(Properties.Settings.Default.SensactDirectory, "firmwareBin", n.Id+".hex");
                cc.Compile(firmwareBinPath);
            }
            
            
            
            return;

        }
    }
}
