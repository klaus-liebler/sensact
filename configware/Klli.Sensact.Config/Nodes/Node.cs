using Klli.Sensact.Config.Applications;
using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Config.Nodes
{

    public abstract class Node
    {

        public Node(ID appId):this(appId.ToString())
        {
           
        }

        public Node(string appId)
        {
            this.Id = appId;
            this.Applications = new List<SensactApplication>();
        }

        public Node()
        {
            
        }

        public string Id;


        public abstract STM32Chip Chip { get; }

        public abstract List<CubeMXModule> CubeMXModules { get; }

        public abstract List<Driver> Drivers { get; }

        public abstract int HSEValue { get; }

        public string BoardType
        {
            get
            {
                string boardId = this.GetType().Name.ToLower();
                return boardId.Substring(0, boardId.Length - 2);
            }
        }

        public virtual int VersionMajor
        {
            get
            {
                string boardId = this.GetType().Name;
                return boardId[boardId.Length - 2] - 0x30;
            }

        }
        public virtual int VersionMinor
        {
            get
            {
                string boardId = this.GetType().Name;
                return boardId[boardId.Length - 1] - 0x30;
            }

        }

        /// <summary>
        /// Idee: Die Applications hier und in der Top-Node sind gleichberechtigt.
        /// Objekte mit gleicher ApplicationId werden zusammengeführt. Priorität hat die Node-spezifische Variante (Hint: Zuerst TopLevel einlesen, dann Node-spezifisch "drüberspielen").
        /// Beim Node-spezifischen wird man primär die IO-Zuordnung machen;
        /// beim TopLevel die restliche Konfiguration
        /// Beim einlesen prüfen, ob jede App genau eine! Node hat und ob Ressourcen nur einmal genutzt werden
        /// </summary>
        [XmlArrayItem(typeof(BlindApplication))]
        [XmlArrayItem(typeof(BrightnessSensorApplication))]
        [XmlArrayItem(typeof(SoundApplication))]
        [XmlArrayItem(typeof(PoweritemApplication))]
        [XmlArrayItem(typeof(PushButtonXApplication))]
        [XmlArrayItem(typeof(PWMApplication))]
        [XmlArrayItem(typeof(RgbwApplication))]
        [XmlArrayItem(typeof(RotaryEncoderApplication))]
        [XmlArrayItem(typeof(StandbyControllerApplication))]
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
        public string ApplicationId;
    }

    public enum RelayMode
    {
        TWO_PHASES = 0, INTERLOCKED_ACTIVE_DOWN = 1, INTERLOCKED_ACTIVE_UP=2
    }


}
