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

            XmlConfigurator.Configure();

            Model model = Sattlerstrasse16.Build();
            if (SourceCodeGenerator.CheckAndPrepare(model))
            {
                SourceCodeGenerator.GenerateAppIds_h(model);
                SourceCodeGenerator.GenerateModel_cpp(model);
                SourceCodeGenerator.GenerateCommandAndEventTypes_h(model);
                SourceCodeGenerator.GenerateApplicationHandCPP(model);
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
