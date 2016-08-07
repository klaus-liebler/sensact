using System.IO;
using log4net.Config;
using Klli.Sensact.Config.Nodes;

namespace Klli.Sensact.Config
{
    public class Program
    {
        private static Model GetModel()
        {
            //return TestModelBuilder.BuildRGBWDemo();
            return TestModelBuilder.BuildModelToTestAllPins();
            //Model model = TestModelBuilder.BuildLIBARDemo();
            //return Sattlerstrasse16.Build();
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
            if(mc!= null)
            {
                SourceCodeGenerator.GenerateAppIds_h(mc);
                SourceCodeGenerator.GenerateModel_cpp(mc);
                SourceCodeGenerator.GenerateCommandAndEventTypes_h(mc);
                SourceCodeGenerator.GenerateApplication_H_and_CPP(mc);
                SourceCodeGenerator.GenerateHeaderIncludesForApplications(mc);
                foreach (Node n in mc.Model.Nodes)
                {
                    SensactFileCollector p = new SensactFileCollector();
                    CompileOptions o = p.BuildCompileOptions(n);
                    CompilerController cc = new CompilerController(o);
                    string firmwareBinPath = Path.Combine(Properties.Settings.Default.SensactDirectory, "firmwareBin", n.Id+".hex");
                    cc.Compile(firmwareBinPath);
                }
            }
            
            
            return;

        }
    }
}
