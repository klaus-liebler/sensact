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
    }

    public class ModelContainer : ModelContainerForCodeGenerator
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
        public Model? Model;

        public string GetNameFromId(ushort id)
        {
            if(id==Model.NO_APPLICATION_ID){
                return "NO_APPLICATION";
            }
            SensactApplicationContainer? cont;
            if(!id2app.TryGetValue(id, out cont)){
                throw new Exception("id "+id +" not found in GetNameFromId");
            }
            return cont.Application.ApplicationName;
        }
    }
}
