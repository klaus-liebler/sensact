using System.Collections.Generic;

namespace Klli.Sensact.Model.Common.Nodes
{
    public class SensactUp04 : Node
    {
        public const int Pin_ROTD = 16+15;
        public const int Pin_xROTD = 16 + 3;
        public const int Pin_0 = 2;
        public const int Pin_1 = 3;
        public const int Pin_2 = 4;
        public const int Pin_3 = 5;
        public const int Pin_4 = 6;
        public const int Pin_5 = 7;
        public const int Pin_6 = 16;
        public const int Pin_7 = 17;
      
           

        public SensactUp04(ushort nodeId, string nodeName) : base(nodeId, nodeName)
        {

        }
        public override STM32Chip Chip
        {
            get
            {
                return new STM32Chip()
                {
                    Family = STM32_FAMILY.F0,
                    Type = STM32_TYPE._72,
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
                return 16000000;
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
                return 4;
            }
        }
    }
}
