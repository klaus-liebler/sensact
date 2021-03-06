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
        public Dictionary<int, SensactApplicationContainer> index2app = new Dictionary<int, SensactApplicationContainer>();
        public Dictionary<string, SensactApplicationContainer> id2app = new Dictionary<string, SensactApplicationContainer>();
        public Dictionary<string, int> predefinedIndices = new Dictionary<string, int>();
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

        internal void PrefillPredefinedIndices(Array enumValues)
        {
            foreach (Nodes.ApplicationId id in enumValues)
            {
                predefinedIndices.Add(id.ToString(), (int)id);
                NextFreeIndex = Math.Max(NextFreeIndex, (int)id + 1);
            }
        }


        internal int GetIndex(string name)
        {
            int foo = 0;
            if (predefinedIndices.TryGetValue(name, out foo))
            {
                return foo;
            }
            else
            {
                int myindex = NextFreeIndex;
                NextFreeIndex++;
                predefinedIndices.Add(name, myindex);
                return myindex;
            }
        }
    }
}
