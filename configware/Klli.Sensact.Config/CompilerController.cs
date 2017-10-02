using Klli.Sensact.Config.Templates;
using log4net;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Klli.Sensact.Config
{
    public class CompileOptions
    {
        public STM32Chip Chip { get; set; }
        public string BoardId { get; set; }
        public int RevMajor { get; set; }
        public int RevMinor { get; set; }
        public string NodeId { get; set; }
        public List<SourceFile> SourceFiles = new List<SourceFile>();
        public List<string> IncDirectories = new List<string>();
        public List<Tuple<string, string>> Defines = new List<Tuple<string, string>>();
        public List<CubeMXModule> CubeMXModulesToInclude = new List<CubeMXModule>();
        public bool UseNewlibNano;
        public bool UseSemihosting;
    }

    public class MCPU
    {
        public const string Cortex_m0plus = "cortex-m0plus";
        public const string Cortex_m0 = "cortex-m0";
        public const string Cortex_m1 = "cortex-m1";
        public const string Cortex_m3 = "cortex-m3";
        public const string Cortex_m4 = "cortex-m4";
    }

    public class SourceFile
    {
        public string Path;
        public Language Language;
        public SourceFile(string path, Language lang)
        {
            Path = path;
            Language = lang;
        }
    }

    public enum STM32_FAMILY { F0 = 0, F1 = 1, F2 = 2, F4 = 4 };
    public enum STM32_TYPE { _00=0, _01=1, _03=3, _05=5, _07=7, _72=72 }
    public enum STM32_PACKAGE { T = 36, C = 48, R = 64, V = 100, Z = 144, I = 176 }
    public enum STM32_FLASH { _4 = 16, _6 = 32, _8 = 64, _B = 128, _C = 256, _D = 384, _E = 512, _F = 768, _G = 1024 }

   

    public enum Language { C=0, CPP=1, ASM=2}

    public class STM32Chip
    {
        public STM32_FAMILY Family { get; set; }
        public STM32_TYPE Type { get; set; }
        public STM32_PACKAGE Package { get; set; } //First package char
        public STM32_FLASH Flash { get; set; } //then flash size char


    }

    public enum CubeMXModule { hal_adc, hal_adc_ex, hal_can, hal_cec, hal_cortex, hal_crc, hal_dac, hal_dac_ex, hal_dma, hal_eth, hal_flash, hal_flash_ex, hal_gpio, hal_gpio_ex, hal_hcd, hal_i2c, hal_i2s, hal_irda, hal_iwdg, hal_nand, hal_nor, hal_pccard, hal_pcd, hal_pwr, hal_rcc, hal_rcc_ex, hal_sd, hal_smartcard, hal_spi, hal_spi_ex, hal_sram, hal_tim, hal_tim_ex, hal_uart, hal_usart, hal_wwdg, ll_fmc, ll_fsmc, ll_sdmmc, ll_usb };
    public enum Driver { BMP180, PCA9555, PCA9685 };


    class CompilerController
    {
        private static ILog LOG = LogManager.GetLogger(typeof(CompilerController));

        public static string TARGET_TRIPLET = "arm-none-eabi";
        public static string EXECUTABLE_FILE_SUFFIX = ".exe";
        public static string C_FLAGS_DEBUG = "-Og -g";
        public static string CXX_FLAGS_DEBUG = "-Og -g";
        public static string ASM_FLAGS_DEBUG = "-g";
        public static string EXE_LINKER_FLAGS_DEBUG = "";

        public static string C_FLAGS_RELEASE = "-Os -flto";
        public static string CXX_FLAGS_RELEASE = "-Os -flto";
        public static string ASM_FLAGS_RELEASE = "";
        public static string EXE_LINKER_FLAGS_RELEASE = "-flto";


        public string toolchainBinDir = Path.Combine(Properties.Settings.Default.ToolchainDirectory, "bin");
        public string toolchainIncDir = Path.Combine(Properties.Settings.Default.ToolchainDirectory, TARGET_TRIPLET, "include");
        public string toolchainLibDir = Path.Combine(Properties.Settings.Default.ToolchainDirectory, TARGET_TRIPLET, "lib");

        private string[] compilerExecutables;

        public string[] family2mcpu = new string[]
       {
            MCPU.Cortex_m0,
            MCPU.Cortex_m3,
            MCPU.Cortex_m3,
            MCPU.Cortex_m3,
            MCPU.Cortex_m4,
       };

        public string[] family2float = new string[]
        {
            string.Empty,
            "-mthumb -mfloat-abi=soft",
            string.Empty,
            string.Empty,
            "-mthumb -mfloat-abi=soft"
        };

        private CompileOptions o;

        private string objCopyExe;
        private string sizeExe;

        public CompilerController(CompileOptions opts)
        {
            compilerExecutables = new string[]
            {
                Path.Combine(toolchainBinDir, TARGET_TRIPLET + "-gcc" + EXECUTABLE_FILE_SUFFIX),
                Path.Combine(toolchainBinDir, TARGET_TRIPLET + "-g++" + EXECUTABLE_FILE_SUFFIX),
                Path.Combine(toolchainBinDir, TARGET_TRIPLET + "-gcc" + EXECUTABLE_FILE_SUFFIX)
            };
            this.objCopyExe = Path.Combine(toolchainBinDir, TARGET_TRIPLET + "-objcopy" + EXECUTABLE_FILE_SUFFIX);
            this.sizeExe = Path.Combine(toolchainBinDir, TARGET_TRIPLET + "-size" + EXECUTABLE_FILE_SUFFIX);
            this.o = opts;

        }
        public static Tuple<string, string>  HSEValue(int freq)
        {
            return new Tuple<string, string>("HSE_VALUE", freq+"");
        }



        //public static string CMAKE_OBJCOPY = Path.Combine(toolchainBinDir, TARGET_TRIPLET + "-objcopy" + EXECUTABLE_FILE_SUFFIX);
        //public static string CMAKE_OBJDUMP = Path.Combine(toolchainBinDir, TARGET_TRIPLET + "-objdump" + EXECUTABLE_FILE_SUFFIX);


        public void Compile(string targetPath)
        {

            o.Defines.Add(new Tuple<string, string>("USE_HAL_DRIVER", null));
            o.Defines.Add(new Tuple<string, string>("STM32", null));
            o.Defines.Add(new Tuple<string, string>(o.BoardId, null));
            o.Defines.Add(new Tuple<string, string>(o.BoardId+o.RevMajor+o.RevMinor, null));
            o.Defines.Add(new Tuple<string, string>("NODE_"+o.NodeId, null));

            switch (o.Chip.Family)
            {
                case STM32_FAMILY.F1:

                    new F1Cube(Properties.Settings.Default.STM32Cube_FW_F1).UpdateOptions(o);
                    break;
                case STM32_FAMILY.F4:
                    new F4Cube(Properties.Settings.Default.STM32Cube_FW_F4).UpdateOptions(o);
                    break;
            }



            LinkerScript ls = new LinkerScript
            {
                CCRamInK = 0,
                CCRamOrigin = 0,
                FlashInK = (uint)o.Chip.Flash,
                RamInK = GetRamInK(o.Chip)
            };


            string tempDir = Path.Combine(Properties.Settings.Default.SensactDirectory, "temp");
            try
            {
                Directory.Delete(tempDir, true);
                Directory.CreateDirectory(tempDir);
            }
            catch (Exception)
            { }
            string s = ls.TransformText();
            string buildDir = tempDir;
            Directory.CreateDirectory(buildDir);
            string linkerScriptPath = Path.Combine(buildDir, "LinkerScript.ld");
            string objectsListPath = Path.Combine(buildDir, "objects.list");
            File.WriteAllText(linkerScriptPath, s);
            using (StreamWriter objects = File.AppendText(objectsListPath))
            {
                foreach (var f in o.SourceFiles)
                {
                    LaunchCompiler(f, objects);

                }
            }
        
            string elfFile = Path.Combine(buildDir, "gen.elf");
            LaunchLinker(linkerScriptPath, objectsListPath, elfFile);
            LaunchObjCopy(elfFile, targetPath);
            LaunchObjSize(elfFile);
            return;
        }

        public uint GetRamInK(STM32Chip chip)
        {
            if(chip.Family== STM32_FAMILY.F1)
            {
                if (chip.Type == STM32_TYPE._03)
                {
                    if (chip.Flash == STM32_FLASH._6)
                        return 10;
                    if (chip.Flash == STM32_FLASH._8 || chip.Flash == STM32_FLASH._B)
                        return 20;
                    if (chip.Flash == STM32_FLASH._E)
                        if (chip.Package == STM32_PACKAGE.C)
                            return 48;
                        else
                            return 54;
                    if (chip.Flash == STM32_FLASH._G)
                        return 96;
                }
            }
            else if(chip.Family==STM32_FAMILY.F4)
            {
                return 192;
            }
           
            return 0;
        }

        private void LaunchObjCopy(string elfFile, string targetPath)
        {

            //-O ihex "sensactF4.elf"  "sensactF4.hex"
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("-O ihex \"{0}\"  \"{1}\"", elfFile, targetPath);
            LOG.InfoFormat("ObjCopying  into file {0}" + Environment.NewLine + "{1} {2}", targetPath, objCopyExe, sb.ToString());
            LOG.Info(LaunchExe(objCopyExe, sb.ToString()));
        }

        private void LaunchObjSize(string elfFile)
        {

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("--format=berkeley \"{0}\"", elfFile);
            LOG.InfoFormat("CalculateSize  of file {0}" + Environment.NewLine + "{1} {2}", elfFile, sizeExe, sb.ToString());
            LOG.Info(LaunchExe(sizeExe, sb.ToString()));
        }

        private string LaunchExe(string exePath, string args)
        {
            StringBuilder sb = new StringBuilder();
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = exePath;
            startInfo.WindowStyle = ProcessWindowStyle.Normal;
            startInfo.Arguments = args;
            startInfo.RedirectStandardError = true;
            startInfo.RedirectStandardOutput = true;
            Process proc = new Process();
            proc.OutputDataReceived += (s, g) => sb.AppendLine(g.Data);
            proc.ErrorDataReceived += (s, g) => sb.AppendLine(g.Data);
            proc.StartInfo = startInfo;
            proc.Start();
            proc.BeginOutputReadLine();
            proc.BeginErrorReadLine();
            proc.WaitForExit();
            if (proc.ExitCode != 0)
            {
                sb.Append(exePath+" exited with code " + proc.ExitCode);
            }
            
            return sb.ToString();
        }

        public void LaunchLinker(string linkerScriptPath, string objectsListPath, string targetPath)
        {
            string exe = compilerExecutables[(int)Language.CPP];
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("-mcpu={0} {1} ", family2mcpu[(int)o.Chip.Family], family2float[(int)o.Chip.Family]);
            sb.Append("-Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wuninitialized -Wmissing-declarations -Wpointer-arith -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 ");
            sb.AppendFormat("-T\"{0}\" ", linkerScriptPath);
            sb.Append("-Xlinker --gc-sections -Wl,-Map,\"output.map\" --specs=nano.specs ");
            sb.AppendFormat("-o \"{0}\" ", targetPath);
            sb.AppendFormat("@\"{0}\" ", objectsListPath);

            if (o.UseNewlibNano)
            {
                sb.Append("--specs=nano.specs ");
            }
            if(o.UseSemihosting)
            {
                sb.Append("--specs=rdimon.specs ");
            }
            else
            {
                sb.Append("--specs=nosys.specs ");
            }
            LOG.InfoFormat("Linking  into file {0}" + Environment.NewLine + "{1} {2}", targetPath, exe, sb);
            LOG.Info(LaunchExe(exe, sb.ToString()));
        }



        public void LaunchCompiler(SourceFile sourceFile, StreamWriter objects)
        {
            StringBuilder sb = new StringBuilder();
            Language lang = sourceFile.Language;

            sb.AppendFormat("-mcpu={0} {1} ", family2mcpu[(int)o.Chip.Family], family2float[(int)o.Chip.Family]);
            foreach (var x in o.Defines)
            {
                sb.Append("-D" + x.Item1);
                if (x.Item2 != null)
                {
                    sb.Append("=" + x.Item2+" ");
                }
                else
                {
                    sb.Append(" ");
                }
            }
            foreach (var i in o.IncDirectories)
            {
                sb.Append("-I\"" + i + "\" ");
            }
            
            switch(lang)
            {
                case Language.C:
                    sb.Append("-Og -g3 -fmessage-length=0 -ffunction-sections -fsigned-char -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wuninitialized -Wmissing-declarations -Wpointer-arith -Wlogical-op -Waggregate-return -Wfloat-equal -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -c ");
                    break;
                case Language.CPP:
                    sb.Append("-Og -g3 -fmessage-length=0 -ffunction-sections -fsigned-char -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wuninitialized -Wmissing-declarations -Wpointer-arith -Wlogical-op -Waggregate-return -Wfloat-equal -c -std=gnu++14 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -Wabi -Wctor-dtor-privacy -Wnoexcept -Wnon-virtual-dtor -Wstrict-null-sentinel -Wsign-promo ");
                    break;
                case Language.ASM:
                    sb.Append("-Og -g3 -fmessage-length=0 -ffunction-sections -fsigned-char -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wuninitialized -Wmissing-declarations -Wpointer-arith -Wlogical-op -Waggregate-return -Wfloat-equal -x assembler-with-cpp -c ");
                    break;   
            }
            string destinationFile = Path.Combine(Properties.Settings.Default.SensactDirectory, "temp", sourceFile.Path.Replace(":", ""));
            destinationFile=Path.ChangeExtension(destinationFile, ".o");
            Directory.CreateDirectory(Path.GetDirectoryName(destinationFile));
            sb.Append("-o \"" + destinationFile + "\" ");
            sb.Append("\"" + sourceFile.Path + "\"");
            string exe = compilerExecutables[(int)lang];
            LOG.InfoFormat("Building File {0}" + Environment.NewLine + "{1} {2}", sourceFile.Path, exe, sb);
            LOG.Info(LaunchExe(exe, sb.ToString()));
            objects.WriteLine("\"" + destinationFile.Replace("\\", "/") + "\"");
        }

        private static void NetOutputDataHandler(object sendingProcess, DataReceivedEventArgs outLine)
        {
            if (!String.IsNullOrEmpty(outLine.Data))
            {
                LOG.Info(outLine.Data);
            }
        }

        private static void NetErrorDataHandler(object sendingProcess,
            DataReceivedEventArgs errLine)
        {
            if (!String.IsNullOrEmpty(errLine.Data))
            {
                LOG.Error(errLine.Data);
            }
        }
    }

    public abstract class CubeMx
    {
        public abstract void UpdateOptions(CompileOptions o);

    }

    public class F1Cube:CubeMx
    {
        private const string HAL_BASE_FILE_PATTERN = "stm32f{0}xx_hal.c";
        private const string HAL_FILE_PATTERN = "stm32f{0}xx_{1}.c";
        private string basePath;

        public F1Cube(string basePath)
        {
            this.basePath = basePath;
        }

        public void AddDefines(CompileOptions o)
        {
            o.Defines.Add(new Tuple<string, string>("STM32F1", null));
            o.Defines.Add(new Tuple<string, string>("STM32F10"+(int)o.Chip.Type+"x"+o.Chip.Flash.ToString()[1], null));
        }

        public void AddHal(CompileOptions o)
        {

            string DriverPath = Path.Combine(basePath, "Drivers", "STM32F1xx_HAL_Driver");
            o.IncDirectories.Add(Path.Combine(DriverPath, "inc"));
            string path = Path.Combine(DriverPath, "src", string.Format(HAL_BASE_FILE_PATTERN, (int)STM32_FAMILY.F1));
            o.SourceFiles.Add(new SourceFile(path, Language.C));
            foreach (CubeMXModule mod in o.CubeMXModulesToInclude)
            {
                path = Path.Combine(DriverPath, "src", string.Format(HAL_FILE_PATTERN, (int)STM32_FAMILY.F1, mod));
                o.SourceFiles.Add(new SourceFile(path, Language.C));
            }
        }

        private const string STARTUP_FILE_PATTERN = "startup_stm32f10{0}x{1}.s";

        public void AddCMSIS(CompileOptions o)
        {
            string DriverPath = Path.Combine(basePath, "Drivers", "CMSIS");
            o.IncDirectories.Add(Path.Combine(DriverPath, "Include"));
            o.IncDirectories.Add(Path.Combine(DriverPath, "Device", "ST", "STM32F1xx", "Include"));
            string srcPath = Path.Combine(DriverPath, "Device", "ST", "STM32F1xx", "Source", "Templates");
            o.SourceFiles.Add(new SourceFile(Path.Combine(srcPath, "system_stm32f1xx.c"), Language.C));
            string startupFilename = string.Format(STARTUP_FILE_PATTERN, (int)o.Chip.Type, o.Chip.Flash.ToString()[1]);
            o.SourceFiles.Add(new SourceFile(Path.Combine(srcPath, "gcc", startupFilename), Language.C));
        }

        public override void UpdateOptions(CompileOptions o)
        {
            AddDefines(o);
            AddCMSIS(o);
            AddHal(o);
            
        }
    }

    public class F4Cube : CubeMx
    {
        private const string HAL_BASE_FILE = "stm32f4xx_hal.c";
        private const string HAL_FILE_PATTERN = "stm32f4xx_{0}.c";
        private string basePath;

        public F4Cube(string basePath)
        {
            this.basePath = basePath;
        }

        private const string STARTUP_FILE_PATTERN = "startup_stm32f40{0}{1}{2}.s";

        public override void UpdateOptions(CompileOptions o)
        {
            o.Defines.Add(new Tuple<string, string>("STM32F4", null));
            o.Defines.Add(new Tuple<string, string>("STM32F40" + (int)o.Chip.Type + "xx", null));

            string CmsisPath = Path.Combine(basePath, "Drivers", "CMSIS");
            o.IncDirectories.Add(Path.Combine(CmsisPath, "Include"));
            o.IncDirectories.Add(Path.Combine(CmsisPath, "Device", "ST", "STM32F4xx", "Include"));
            string srcPath = Path.Combine(CmsisPath, "Device", "ST", "STM32F4xx", "Source", "Templates");
            o.SourceFiles.Add(new SourceFile(Path.Combine(srcPath, "system_stm32f4xx.c"), Language.C));
            string startupFilename = string.Format(STARTUP_FILE_PATTERN, (int)o.Chip.Type, o.Chip.Package.ToString().ToLower(), o.Chip.Flash.ToString().ToLower()[1]);
            string startupFilePath = Path.Combine(srcPath, "gcc", startupFilename);
            if(!File.Exists(startupFilePath))
            {
                startupFilename = string.Format(STARTUP_FILE_PATTERN, (int)o.Chip.Type, "x", o.Chip.Flash.ToString().ToLower()[1]);
                startupFilePath = Path.Combine(srcPath, "gcc", startupFilename);
                if (!File.Exists(startupFilePath))
                {
                    startupFilename = string.Format(STARTUP_FILE_PATTERN, (int)o.Chip.Type, o.Chip.Package.ToString().ToLower(), "x");
                    startupFilePath = Path.Combine(srcPath, "gcc", startupFilename);
                    if (!File.Exists(startupFilePath))
                    {
                        startupFilename = string.Format(STARTUP_FILE_PATTERN, (int)o.Chip.Type, "x", "x");
                        startupFilePath = Path.Combine(srcPath, "gcc", startupFilename);
                    }
                }
            }
            o.SourceFiles.Add(new SourceFile(startupFilePath, Language.C));

            string HalPath = Path.Combine(basePath, "Drivers", "STM32F4xx_HAL_Driver");
            o.IncDirectories.Add(Path.Combine(HalPath, "Inc"));
            string path = Path.Combine(HalPath, "Src", HAL_BASE_FILE);
            o.SourceFiles.Add(new SourceFile(path, Language.C));
            foreach (CubeMXModule mod in o.CubeMXModulesToInclude)
            {
                path = Path.Combine(HalPath, "src", string.Format(HAL_FILE_PATTERN, mod));
                o.SourceFiles.Add(new SourceFile(path, Language.C));
            }

        }
    }
}
