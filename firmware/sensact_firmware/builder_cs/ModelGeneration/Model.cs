using System.Xml.Serialization;
using Klli.Sensact.Model.Common.Nodes;

namespace Klli.Sensact.Model.Common
{
    public class Model(string name)
    {       
        [XmlElement(typeof(SensactHs07))]
        [XmlElement(typeof(SensactUp02))]
        public List<Node> Nodes = new List<Node>();
        
        public string Name { get; }=name;

        public const ushort NO_APPLICATION_ID=0xFFFF;
        public const ushort MASTER_APPLICATION_ID=0x0000;
    }
}
