using Klli.Sensact.Model.Common.Applications;
using System.Xml.Serialization;

namespace Klli.Sensact.Model.Common.Nodes
{

    public abstract class Node
    {
        public Node(ushort nodeId,  string nodeName, bool NodeHasANodeApplication=true)
        {
            this.NodeId = nodeId;
            this.NodeName=nodeName;
            this.NodeHasANodeApplication=NodeHasANodeApplication;
            this.Applications = new List<SensactApplication>();
        }


        public string NodeName{get;}
        public ushort NodeId{get;}

        public bool NodeHasANodeApplication{get;}


        /// <summary>
        /// Idee: Die Applications hier und in der Top-Node sind gleichberechtigt.
        /// Objekte mit gleicher ApplicationId werden zusammengeführt. Priorität hat die Node-spezifische Variante (Hint: Zuerst TopLevel einlesen, dann Node-spezifisch "drüberspielen").
        /// Beim Node-spezifischen wird man primär die IO-Zuordnung machen;
        /// beim TopLevel die restliche Konfiguration
        /// Beim einlesen prüfen, ob jede App genau eine! Node hat und ob Ressourcen nur einmal genutzt werden
        /// </summary>
        [XmlArrayItem(typeof(BlindApplication))]
        [XmlArrayItem(typeof(OnOffApplication))]
        [XmlArrayItem(typeof(SinglePWMApplication))]
        [XmlArrayItem(typeof(RgbwApplication))]
        [XmlArrayItem(typeof(SoundApplication))]
        [XmlArrayItem(typeof(PumpApplication))]
        [XmlArrayItem(typeof(PushButtonSingle2PwmSingleApplication))]
        [XmlArrayItem(typeof(PushButtonSingle2ToggleApplication))]
        [XmlArrayItem(typeof(PushButtonDual2BlindApplication))]
        [XmlArrayItem(typeof(PushButtonDual2PWMApplication))]
        [XmlArrayItem(typeof(RotaryEncoder2PWMApplication))]
        [XmlArrayItem(typeof(BrightnessSensorApplication))]
        
        public List<SensactApplication> Applications;

    }

    public enum RessourceCapability { PushButton, Relay, PWM, DHT22 }



    //Ports=Pins am Prozessor bzw Porterweiterungen durch I2C oder SPI

    //CPU-Functions (Timer-Blöcke, I2C-Pins)


    public enum RotaryEncoder
    {
        ROTARYENCODER_1,
        ROTARYENCODER_2,
    }

    public class RessourceMapEntry
    {
        public string ApplicationId=string.Empty;
    }

    public enum RelayInterlockMode
    {
        R1_UP__R2_DOWN = 0,
        R1_DOWN__R2_UP = 1,
        R1_POWER__R2_UP = 2,
        R1_POWER__R2_DOWN = 3,
        R1_UP__R2_POWER = 4,
        R1_DOWN__R2_POWER = 5,

    };

    public enum PowerState{
        INACTIVE = 0,
        ACTIVE = 0xFFFF,
    };


}
