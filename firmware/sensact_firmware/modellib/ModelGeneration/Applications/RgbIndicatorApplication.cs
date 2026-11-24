using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    /// <summary>
    /// Signal- und Orientierungslicht aus adressierbaren RGB-LEDs der Station (cRgbIndicator):
    /// Dauerlicht (ON/OFF/TOGGLE, SET_RGBW, SET_VERTICAL_TARGET) oder ein Signalmuster (SET_SIGNAL:
    /// 1 Tuerklingel, 2 Alarm, 3 Hinweis, 4 OK, 5 Fehler).
    /// </summary>
    public class RgbIndicatorApplication : ActorApplication
    {
        public RgbIndicatorApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription,
            IReadOnlyList<byte> leds, uint color, ushort toggleSignal, uint autoOffMsecs)
            : base(ApplicationId, ApplicationName, ApplicationDescription)
        {
            Leds = leds;
            Color = color;
            ToggleSignal = toggleSignal;
            AutoOffMsecs = autoOffMsecs;
        }

        /// <summary>Indizes der RGB-LEDs der Station (Katalog-Feature rgbLed).</summary>
        public IReadOnlyList<byte> Leds { get; }
        /// <summary>Farbe des Dauerlichts, 0xRRGGBB.</summary>
        public uint Color { get; }
        /// <summary>Was TOGGLE tut: 0 Dauerlicht an/aus, sonst dieses Signalmuster.</summary>
        public ushort ToggleSignal { get; }
        public uint AutoOffMsecs { get; }

        [SensactCommandMethod]
        public override void OnONCommand(uint autoReturnToOffMsecs) { base.OnONCommand(autoReturnToOffMsecs); }
        [SensactCommandMethod]
        public override void OnOFFCommand(uint autoReturnToOnMsecs) { base.OnOFFCommand(autoReturnToOnMsecs); }
        [SensactCommandMethod]
        public override void OnTOGGLECommand() { base.OnTOGGLECommand(); }
        [SensactCommandMethod]
        public override void OnSET_RGBWCommand(byte R, byte G, byte B, byte W) { base.OnSET_RGBWCommand(R, G, B, W); }
        [SensactCommandMethod]
        public override void OnSET_VERTICAL_TARGETCommand(ushort target) { base.OnSET_VERTICAL_TARGETCommand(target); }
        [SensactCommandMethod]
        public override void OnSET_SIGNALCommand(ushort signal) { base.OnSET_SIGNALCommand(signal); }

        public override HashSet<EventType> ICanSendTheseEvents() => new HashSet<EventType>();

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            var sb = new StringBuilder();
            sb.AppendFormat("sensact::apps::cRgbIndicator {0}(eApplicationID::{0}, {{{1}}}, 0x{2:X6}, {3}, {4})",
                ApplicationName, string.Join(", ", Leds), Color, ToggleSignal, AutoOffMsecs);
            return sb.ToString();
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m) => string.Empty;

        // RGB-LEDs sind keine Ein-/Ausgaenge der Busse; doppelte Belegung prueft der HouseModelChecker
        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins) => string.Empty;

        public override Regex AppNameRegex => FLOOR_ROOM_Regex("SIGNL");
    }
}
