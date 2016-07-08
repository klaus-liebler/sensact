using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Config
{
    public class Model
    {       
        [XmlElement(typeof(SensactHs07))]
        [XmlElement(typeof(SensactUp02))]
        public List<Node> Nodes = new List<Node>();
    }

    public class ModelContainer
    {
        public Dictionary<int, SensactApplicationContainer> index2app = new Dictionary<int, SensactApplicationContainer>();
        public Dictionary<string, SensactApplicationContainer> id2app = new Dictionary<string, SensactApplicationContainer>();
        /// <summary>
        /// 0 ist reservierter Index für "MASTER_APP", Echte AppIndices beginnen bei 1
        /// </summary>
        /// <summary>
        /// speichert, welche events von der betreffenden ApplicationId von anderen Apps gehört werden und deshalb lokal bzw auf den Bus nach außen zu geben sind
        /// </summary>
        public Dictionary<string, HashSet<EventType>> id2localEvents = new Dictionary<string, HashSet<EventType>>();
        public Dictionary<string, HashSet<EventType>> id2busEvents = new Dictionary<string, HashSet<EventType>>();
        public Model Model;
        public int NextFreeIndex;
    }
}
