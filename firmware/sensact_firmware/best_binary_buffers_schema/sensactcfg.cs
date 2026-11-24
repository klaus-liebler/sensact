// Konfiguration einer Station (s. docs/concept-modular-firmware.md, Schritt 2 "datengetriebene
// Basis"): welche Busse/Erweiterungen und welche Apps die Basis-Firmware beim Start anlegt. Erzeugt
// vom Builder aus dem XML-Hausmodell (builder/StationConfigWriter.cs, BestBinaryBuffers.Runtime.
// BinaryEncoder), Abschnitt 2 des Stationspakets; gelesen von der App-Fabrik der Firmware
// (sensact-applicationmodel/station_config.cc) mit dem hieraus generierten C++-Decoder.
//
// Alle Werte stehen hier in der internen Darstellung der Firmware (InOutId, Millisekunden,
// u16-Pegel) -- die Umrechnung aus den lesbaren Einheiten des Hausmodells macht der Builder.
// Felder nur hinten anfuegen bzw. FormatVersion erhoehen: eine Basis versteht nur Pakete, deren
// Schema sie kennt (s. compatId im Stationspaket).
using BestBinaryBuffers;

namespace sensactcfg;

/// Verriegelung der beiden Relais eines Rollladens -- Reihenfolge wie eRelayInterlockMode in
/// sensactcommon/sensact_commons.hh (dort per static_assert abgesichert).
[BinaryType]
public enum RelayInterlockMode : byte
{
	R1_POWER__R2_UP,
	R1_POWER__R2_DOWN,
	R1_UP__R2_POWER,
	R1_DOWN__R2_POWER,
	R1_DOWN__R2_UP,
	R1_UP__R2_DOWN,
}

/// Daemmerungsart fuer cBlindsTimer (sunsetsunrise::eDawn)
[BinaryType]
public enum Dawn : byte
{
	NORMAL,
	CIVIL,
	NAUTIC,
	ASTRONOMIC,
}

// ------------------------------------------------------------------------------- Busse

[BinaryUnion]
public interface IBus
{
}

/// Direkte Ein-/Ausgaenge der Board-HAL (Busmaster "GPIO").
[BinaryType]
public class GpioBus : IBus
{
	/// Bus-Index 0..3 (die oberen zwei Bits der InOutId)
	public byte Index;
}

/// I2C-Bus mit PCA9555 (Eingaenge) und PCA9685 (Ausgaenge); Reihenfolge = Chip-Index.
[BinaryType]
public class I2cBus : IBus
{
	public byte Index;
	/// 7-Bit-Adressen der PCA9555 (0x20..0x27)
	[BinaryMaxItemCount(8)] public byte[] Pca9555;
	/// 7-Bit-Adressen der PCA9685 (0x40..0x7F)
	[BinaryMaxItemCount(16)] public byte[] Pca9685;
}

// ------------------------------------------------------------------------------- Apps
// Je App-Typ eine Klasse, Felder in der Reihenfolge der Konstruktorparameter der C++-App.
// App-Nummern als ushort (65535 = NO_APPLICATION).

[BinaryUnion]
public interface IApp
{
}

[BinaryType]
public class NodeApp : IApp
{
	public ushort Id;
}

[BinaryType]
public class BlindApp : IApp
{
	public ushort Id;
	public ushort Relay1;
	public ushort Relay2;
	public RelayInterlockMode Mode;
	public uint UpTimeMs;
	public uint DownTimeMs;
}

[BinaryType]
public class OnOffApp : IApp
{
	public ushort Id;
	public ushort Output;
	public bool InitiallyOn;
	public uint AutoOffMs;
}

[BinaryType]
public class SinglePwmApp : IApp
{
	public ushort Id;
	[BinaryMaxItemCount(16)] public ushort[] Outputs;
	public ushort MinimalLevel;
	public ushort InitialLevel;
	public uint AutoOffMs;
	public ushort StandbyController;
}

[BinaryType]
public class RgbwApp : IApp
{
	public ushort Id;
	public ushort OutputR;
	public ushort OutputG;
	public ushort OutputB;
	public ushort OutputW;
	public uint AutoOffMs;
	public ushort StandbyController;
}

[BinaryType]
public class SoundApp : IApp
{
	public ushort Id;
	public ushort InitialVolume;
	public ushort DefaultSignalOnToggle;
}

/// Signal-/Orientierungslicht aus RGB-LEDs der Station (cRgbIndicator)
[BinaryType]
public class RgbIndicatorApp : IApp
{
	public ushort Id;
	/// Indizes der LEDs (SetRGBLed)
	[BinaryMaxItemCount(16)] public byte[] Leds;
	/// Farbe des Dauerlichts, 0xRRGGBB
	public uint Color;
	/// TOGGLE: 0 Dauerlicht an/aus, sonst dieses Signalmuster
	public ushort ToggleSignal;
	public uint AutoOffMs;
}

[BinaryType]
public class PumpApp : IApp
{
	public ushort Id;
	public ushort Output;
	public uint OnIntervalMs;
	public uint OffIntervalMs;
}

[BinaryType]
public class ToggleButtonApp : IApp
{
	public ushort Id;
	public ushort Input;
	[BinaryMaxItemCount(16)] public ushort[] Targets;
}

[BinaryType]
public class DimButtonApp : IApp
{
	public ushort Id;
	public ushort Input;
	public ushort Target;
}

[BinaryType]
public class DualDimButtonsApp : IApp
{
	public ushort Id;
	public ushort InputUp;
	public ushort InputDown;
	public ushort Target;
}

[BinaryType]
public class BlindButtonsApp : IApp
{
	public ushort Id;
	public ushort InputUp;
	public ushort InputDown;
	public ushort Target;
}

[BinaryType]
public class RotaryEncoderApp : IApp
{
	public ushort Id;
	/// 0 = ROTENC0, 1 = ROTENC1
	public byte Encoder;
	public ushort Target;
}

[BinaryType]
public class LightbarrierApp : IApp
{
	public ushort Id;
	public ushort Input;
	public bool ActiveHigh;
	public ushort BrightnessSensor;
	public ushort Target;
}

[BinaryType]
public class FingerprintApp : IApp
{
	public ushort Id;
	public ushort Target1;
	public ushort Target2;
	public ushort Target3;
}

[BinaryType]
public class MilightControllerApp : IApp
{
	public ushort Id;
	public ushort Light;
	[BinaryMaxItemCount(8)] public ushort[] Blinds;
	public uint LightOnMs;
}

[BinaryType]
public class BlindsTimerApp : IApp
{
	public ushort Id;
	[BinaryMaxItemCount(32)] public ushort[] Blinds;
	/// Breite/Laenge in Mikrograd (exakt, BestBinaryBuffers kennt kein double)
	public int LatitudeMicroDeg;
	public int LongitudeMicroDeg;
	public Dawn Dawn;
	public short OpenOffsetMin;
	public short CloseOffsetMin;
	public ushort MaxRandomMin;
}

// ------------------------------------------------------------------------------- Namen

[BinaryUnion]
public interface IAppName
{
}

/// Name einer App des Hauses (fuer Log-Ausgaben, auch fuer Apps anderer Stationen)
[BinaryType]
public class AppName : IAppName
{
	public ushort Id;
	[BinaryMaxEncodedByteLength(31)] public string Name;
}

// ------------------------------------------------------------------------------- Station

[BinaryMessage(MessageKind.Event)]
public class StationConfig
{
	/// Version dieses Schemas (1)
	public ushort FormatVersion;
	/// Wert von eNodeID dieser Station (Position in der Stationsliste des Hausmodells)
	public ushort NodeId;
	/// Nummer der Station als App (Node-App)
	public ushort NodeAppId;
	[BinaryMaxEncodedByteLength(31)] public string NodeName;
	[BinaryMaxEncodedByteLength(63)] public string HouseName;
	/// Erzeugungszeitpunkt, lesbar
	[BinaryMaxEncodedByteLength(31)] public string Created;
	/// groesste App-Nummer des Hauses + 1 (Groesse der App-Tabellen)
	public ushort AppIdCount;
	[BinaryMaxItemCount(4)] public IBus[] Buses;
	[BinaryMaxItemCount(192)] public IApp[] Apps;
	[BinaryMaxItemCount(1024)] public IAppName[] Names;
}
