using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Config
{
    public class Model
    {       
        [XmlElement(typeof(SensactHs07))]
        [XmlElement(typeof(SensactUp02))]
        public List<Node> Nodes = new List<Node>();
        public Model(string name)
        {
            Name = name;
        }
        public string Name { get; private set; }
    }

    public class ModelContainer
    {
        public Dictionary<ushort, SensactApplicationContainer> id2app = new Dictionary<ushort, SensactApplicationContainer>();
        
        /// <summary>
        /// 0 ist reservierter Index für "MASTER_APP", Echte AppIndices beginnen bei 1
        /// </summary>
        /// <summary>
        /// speichert, welche events von der betreffenden ApplicationId von anderen Apps gehört werden und deshalb lokal bzw auf den Bus nach außen zu geben sind
        /// </summary>
        public Dictionary<ushort, HashSet<EventType>> id2localEvents = new Dictionary<ushort, HashSet<EventType>>();
        public Dictionary<ushort, HashSet<EventType>> id2busEvents = new Dictionary<ushort, HashSet<EventType>>();

        public ushort NextFreeIndex=0;
        public Model Model;
    }
}
