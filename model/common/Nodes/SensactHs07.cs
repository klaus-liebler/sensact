using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Model.Common.Nodes
{
    public class SensactHs07 : Node
    {
        public SensactHs07(ushort nodeId, string nodeName) : base(nodeId, nodeName)
        {

        }

        public override int VersionMajor
        {
            get
            {
                return 0;
            }
        }

        public override int VersionMinor
        {
            get
            {
                return 7;
            }
        }

        public override List<CubeMXModule> CubeMXModules
        {
            get
            {
                return new List<CubeMXModule>
                {
                    CubeMXModule.hal_gpio,
                    CubeMXModule.hal_rcc,
                    CubeMXModule.hal_cortex,
                    CubeMXModule.hal_i2c,
                    CubeMXModule.hal_spi,
                    CubeMXModule.hal_tim,
                    CubeMXModule.hal_uart,
                    CubeMXModule.hal_can,
                    CubeMXModule.hal_dma,
                    CubeMXModule.hal_tim_ex,
                    CubeMXModule.hal_sd,
                    CubeMXModule.ll_sdmmc,
                };
            }
        }

        public override List<Driver> Drivers
        {
            get
            {
                return new List<Driver>
                {
                    Driver.PCA9555, Driver.PCA9685
                };
            }
        }

        public override STM32Chip Chip
        {
            get
            {
                return new STM32Chip()
                {
                    Family = STM32_FAMILY.F4,
                    Type = STM32_TYPE._05,
                    Package = STM32_PACKAGE.R,
                    Flash = STM32_FLASH._G
                };
            }

        }

        public override int HSEValue
        {
            get
            {
                return 8000000;
            }
        }
    }
}
