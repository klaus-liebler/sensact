using Klli.Sensact.Config.Applications;
using System.Collections.Generic;
using System.Xml.Serialization;

namespace Klli.Sensact.Config.Nodes
{

    public abstract class Node
    {
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

        [XmlIgnore]
        public static InputPin[] index2input =   { InputPin.I01, InputPin.I01, InputPin.I02, InputPin.I03, InputPin.I04, InputPin.I05, InputPin.I06, InputPin.I07, InputPin.I08, InputPin.I09, InputPin.I10, InputPin.I11, InputPin.I12, InputPin.I13, InputPin.I14, InputPin.I15, InputPin.I16, InputPin.I17, InputPin.I18, InputPin.I19, InputPin.I20, InputPin.I21, InputPin.I22, InputPin.I23, InputPin.I24, InputPin.I25, InputPin.I26, InputPin.I27, InputPin.I28, InputPin.I29, InputPin.I30, InputPin.I31, InputPin.I32 };

        [XmlIgnore]
        public static OutputPin[] index2output = { OutputPin.O01, OutputPin.O01,OutputPin.O02, OutputPin.O03, OutputPin.O04, OutputPin.O05, OutputPin.O06, OutputPin.O07, OutputPin.O08,OutputPin.O09,OutputPin.O10,OutputPin.O11,OutputPin.O12, OutputPin.O13,  OutputPin.O14, OutputPin.O15,  OutputPin.O16, OutputPin.O17, OutputPin.O18, OutputPin.O19, OutputPin.O20, OutputPin.O21, OutputPin.O22, OutputPin.O23, OutputPin.O24, OutputPin.O25, OutputPin.O26, OutputPin.O27, OutputPin.O28, OutputPin.O29, OutputPin.O30, OutputPin.O31, OutputPin.O32 };


        [XmlIgnore]
        public static PwmPin[] index2pwm = {
            PwmPin.P01,//doppelt, is OK!
        PwmPin.P01,
        PwmPin.P02,
        PwmPin.P03,
        PwmPin.P04,
        PwmPin.P05,
        PwmPin.P06,
        PwmPin.P07,
        PwmPin.P08,
        PwmPin.P09,
        PwmPin.P10,
        PwmPin.P11,
        PwmPin.P12,
        PwmPin.P13,
        PwmPin.P14,
        PwmPin.P15,
        PwmPin.P16,
        PwmPin.P17,
        PwmPin.P18,
        PwmPin.P19,
        PwmPin.P20,
        PwmPin.P21,
        PwmPin.P22,
        PwmPin.P23,
        PwmPin.P24,
        PwmPin.P25,
        PwmPin.P26,
        PwmPin.P27,
        PwmPin.P28,
        PwmPin.P29,
        PwmPin.P30,
        PwmPin.P31,
        PwmPin.P32,
        };


        /// <summary>
        /// Idee: Die Applications hier und in der Top-Node sind gleichberechtigt.
        /// Objekte mit gleicher ApplicationId werden zusammengeführt. Priorität hat die Node-spezifische Variante (Hint: Zuerst TopLevel einlesen, dann Node-spezifisch "drüberspielen").
        /// Beim Node-spezifischen wird man primär die IO-Zuordnung machen;
        /// beim TopLevel die restliche Konfiguration
        /// Beim einlesen prüfen, ob jede App genau eine! Node hat und ob Ressourcen nur einmal genutzt werden
        /// </summary>
        [XmlArrayItem(typeof(BlindApplication))]
        [XmlArrayItem(typeof(BrightnessSensorApplication))]
        [XmlArrayItem(typeof(DoorbellApplication))]
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

    public enum InputPin
    {
        I01, I02, I03, I04, I05, I06, I07, I08, I09, I10, I11, I12, I13, I14, I15, I16, I17, I18, I19, I20, I21, I22, I23, I24, I25, I26, I27, I28, I29, I30, I31, I32, ROTAR_PUSH_1, ROTAR_PUSH_2, II01, II02, II03, II04, II05, II06, II07, II08, II09, II10, II11, II12, II13
    }



    public enum OutputPin
    {
        O01,
        O02,
        O03,
        O04,
        O05,
        O06,
        O07,
        O08,
        O09,
        O10,
        O11,
        O12,
        O13,
        O14,
        O15,
        O16,
        O17,
        O18,
        O19,
        O20,
        O21,
        O22,
        O23,
        O24,
        O25,
        O26,
        O27,
        O28,
        O29,
        O30,
        O31,
        O32,
        O_LED,
    }

    public enum PwmPin
    {
        P01,
        P02,
        P03,
        P04,
        P05,
        P06,
        P07,
        P08,
        P09,
        P10,
        P11,
        P12,
        P13,
        P14,
        P15,
        P16,
        P17,
        P18,
        P19,
        P20,
        P21,
        P22,
        P23,
        P24,
        P25,
        P26,
        P27,
        P28,
        P29,
        P30,
        P31,
        P32,
        OP01,
        OP02,
        OP03,
        OP04,
        OP05,
        OP06,
        OP07,
        OP08,
        NONE,
    }

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
        TWO_PHASES = 0, INTERLOCKED = 1,
    }


}
