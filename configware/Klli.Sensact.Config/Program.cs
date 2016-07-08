using System;
using System.Collections.Generic;
using System.IO;
using log4net.Config;
using Klli.Sensact.Config.Nodes;

namespace Klli.Sensact.Config
{
    class Program
    {
        static void Main(string[] args)
        {
            int w = (30 + 1 - 2) % 3;

            XmlConfigurator.Configure();

            Model model = Sattlerstrasse16.Build();
            ModelContainer mc = new ModelContainer()
            {
                Model = model
            };
            if (SourceCodeGenerator.CheckAndPrepare(mc))
            {
                SourceCodeGenerator.GenerateAppIds_h(mc);
                SourceCodeGenerator.GenerateModel_cpp(mc);
                SourceCodeGenerator.GenerateCommandAndEventTypes_h(mc);
                SourceCodeGenerator.GenerateApplicationHandCPP(mc);
                foreach (Node n in model.Nodes)
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
