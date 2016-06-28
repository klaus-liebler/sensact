using System.Collections.Generic;

namespace Klli.Sensact.Config.Nodes
{
    class SensactUp02 : Node
    {
        public override STM32Chip Chip
        {
            get
            {
                return new STM32Chip()
                {
                    Family = STM32_FAMILY.F1,
                    Type = STM32_TYPE._03,
                    Package = STM32_PACKAGE.C,
                    Flash = STM32_FLASH._B
                };
            }

        }

        public override List<CubeMXModule> CubeMXModules
        {
            get
            {
                return new List<CubeMXModule>
                {
                    CubeMXModule.hal_flash, CubeMXModule.hal_gpio, CubeMXModule.hal_rcc, CubeMXModule.hal_cortex, CubeMXModule.hal_i2c
                };
            }
        }

        public override List<Driver> Drivers
        {
            get
            {
                return new List<Driver>
                {
                    Driver.BMP180
                };
            }
        }

        public override int HSEValue
        {
            get
            {
                return 12000000;
            }
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
                return 2;
            }
        }
    }
}
