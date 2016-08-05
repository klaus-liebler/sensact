using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Config.Nodes
{
    public class SensactHs04 : Node
    {
        public SensactHs04()
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
                return 4;
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

        [XmlIgnore]
        public static Dictionary<InputPin, List<RessourceCapability>> Ressources;
        static SensactHs04()
        {
            Ressources = new Dictionary<InputPin, List<RessourceCapability>>();
            Ressources.Add(InputPin.I01, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I02, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I03, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I04, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I05, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I06, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I07, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I08, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I09, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I10, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I11, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I12, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I13, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I14, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I15, new List<RessourceCapability>() { RessourceCapability.PushButton });
            Ressources.Add(InputPin.I16, new List<RessourceCapability>() { RessourceCapability.PushButton });
        }
    }
}
