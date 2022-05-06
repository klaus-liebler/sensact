using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Config.Nodes
{
    public class SensactHs08 : SensactHs07
    {
        public SensactHs08(ApplicationId appId) : base((ushort)appId, appId.ToString())
        {

        }

        public SensactHs08(ushort nodeId, string nodeName) : base(nodeId, nodeName)
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
                return 8;
            }
        }

        

        public override STM32Chip Chip
        {
            get
            {
                return new STM32Chip()
                {
                    Family = STM32_FAMILY.F4,
                    Type = STM32_TYPE._07,
                    Package = STM32_PACKAGE.V,
                    Flash = STM32_FLASH._E
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
