using System.Collections.Generic;

namespace Klli.Sensact.Config.Nodes
{
    internal class SensactUp02 : Node
    {
        public const int Pin_ROTD = 16+15;
        public const int Pin_xROTD = 16 + 3;
        public const int IO1 = 16;
        public const int IO2 = 17;
        public const int IO3 = 6;
        public const int IO4 = 7;
        public const int IO5 = 4;
        public const int IO6 = 5;
        public const int IO7 = 2;
        public const int IO8 = 3;
                public SensactUp02(ApplicationId appId) : base((ushort)appId, appId.ToString())
        {

        }

        public SensactUp02(ushort nodeId, string nodeName) : base(nodeId, nodeName)
        {

        }
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
