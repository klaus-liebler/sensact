
namespace Klli.Sensact.Model.Common
{
    public class CompileOptions
    {
        public STM32Chip Chip { get; set; }
        public string BoardId { get; set; }
        public int RevMajor { get; set; }
        public int RevMinor { get; set; }
        public string? NodeId { get; set; }
        public List<SourceFile> SourceFiles = new List<SourceFile>();
        public List<string> IncDirectories = new List<string>();
        public List<Tuple<string, string>> Defines = new List<Tuple<string, string>>();
        public List<CubeMXModule> CubeMXModulesToInclude = new List<CubeMXModule>();
        public bool UseNewlibNano;
        public bool UseSemihosting;

        public CompileOptions(){
            Chip=new STM32Chip();
            BoardId=string.Empty;
        }
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
            o.Defines.Add(new Tuple<string, string>("STM32F1", string.Empty));
            o.Defines.Add(new Tuple<string, string>("STM32F10"+(int)o.Chip.Type+"x"+o.Chip.Flash.ToString()[1], string.Empty));
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
            o.Defines.Add(new Tuple<string, string>("STM32F4", string.Empty));
            o.Defines.Add(new Tuple<string, string>("STM32F40" + (int)o.Chip.Type + "xx", string.Empty));

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
