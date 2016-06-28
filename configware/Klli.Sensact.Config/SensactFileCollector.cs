using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.IO;


namespace Klli.Sensact.Config
{
    public class SensactFileCollector
    {
        public CompileOptions O { get; private set; }
        public CompileOptions BuildCompileOptions(Node node)
        {
            O = new CompileOptions()
            {
                BoardId = node.BoardType.ToUpper(),
                RevMajor = node.VersionMajor,
                RevMinor = node.VersionMinor,
                NodeId = node.Id,
                Chip = node.Chip,
                Defines =
                {
                    CompilerController.HSEValue(node.HSEValue)
                },
                CubeMXModulesToInclude = node.CubeMXModules,
                IncDirectories = new List<string>()
            };
            string firmwaredir = Path.Combine(Properties.Settings.Default.SensactDirectory, "firmware");
            string moddir = Path.Combine(firmwaredir, "modules");
            string bspDir = Path.Combine(moddir, "bsp", node.BoardType);
            O.IncDirectories.Add(bspDir);         
            foreach (string file in Directory.EnumerateFiles(bspDir))
            {
                AddFile(file);
            }
          

            string genDir = Path.Combine(moddir, "generated");
            O.IncDirectories.Add(genDir);
            //check if there are hw specific files for each file in the bsp directory            
            foreach (string file in Directory.EnumerateFiles(genDir, "*.cpp", SearchOption.TopDirectoryOnly))
            {
                AddFile(file);
            }
            string driversDir = Path.Combine(moddir, "drivers");
            O.IncDirectories.Add(driversDir);         
            foreach (Driver driver in  node.Drivers)
            {
                string file = Path.Combine(driversDir, Convert.ToString(driver) + ".cpp");
                if (File.Exists(file))
                {
                    AddFile(file);
                }
                else
                {
                    throw new Exception("HAL file "+file+" does not exist");
                }

            }
            string coreDir = Path.Combine(moddir, "core");
            O.IncDirectories.Add(coreDir);
            foreach (string file in Directory.EnumerateFiles(coreDir, "*.cpp", SearchOption.TopDirectoryOnly))
            {
                AddFile(file);
            }

            return O;
        }

        private void AddFile(string file)
        {
            string ext = Path.GetExtension(file).ToLower();
            switch (ext)
            {
                case ".cpp":
                    O.SourceFiles.Add(new SourceFile(file, Language.CPP));
                    break;
                case ".c":
                    O.SourceFiles.Add(new SourceFile(file, Language.C));
                    break;
                case ".s":
                case ".S":
                case ".asm":
                    O.SourceFiles.Add(new SourceFile(file, Language.ASM));
                    break;
                default:
                    break;
            }
        }

    }
}
