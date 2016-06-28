using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Config
{
    public class Model
    {
        [XmlIgnore]
        public Dictionary<string, SensactApplication> id2app = new Dictionary<string, SensactApplication>();
        /// <summary>
        /// 0 ist reservierter Index für "MASTER_APP", Echte AppIndices beginnen bei 1
        /// </summary>
        [XmlIgnore]
        public Dictionary<string, int> id2index = new Dictionary<string, int>();
        [XmlIgnore]
        public SensactApplication[] index2app;


        /// <summary>
        /// speichert, welche events von der betreffenden ApplicationId von anderen Apps gehört werden und deshalb lokal bzw auf den Bus nach außen zu geben sind
        /// </summary>
        [XmlIgnore]
        public Dictionary<string, HashSet<EventType>> id2localEvents = new Dictionary<string, HashSet<EventType>>();
        [XmlIgnore]
        public Dictionary<string, HashSet<EventType>> id2busEvents = new Dictionary<string, HashSet<EventType>>();
        
        [XmlElement(typeof(SensactHs07))]
        [XmlElement(typeof(SensactUp02))]
        public List<Node> Nodes = new List<Node>();

        [XmlArrayItem(typeof(BlindApplication))]
        [XmlArrayItem(typeof(BrightnessSensorApplication))]
        [XmlArrayItem(typeof(DoorbellApplication))]
        [XmlArrayItem(typeof(PoweritemApplication))]
        [XmlArrayItem(typeof(PushButtonXApplication))]
        [XmlArrayItem(typeof(PWMApplication))]
        [XmlArrayItem(typeof(RgbwApplication))]
        [XmlArrayItem(typeof(RotaryEncoderApplication))]
        [XmlArrayItem(typeof(StandbyController))]
        public List<SensactApplication> Applications = new List<SensactApplication>();
    }
}
