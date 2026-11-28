// Firmware-Update der sensactUp-Stationen (STM32F103) ueber den CAN-Bus: Speicheraufteilung,
// Image-Kopf, CAN-Protokoll und der Empfangsautomat des Bootloaders.
//
// Ohne Abhaengigkeit von rtoos oder der Hardware: dieselbe Datei baut der Bootloader
// (bootloader/main.cpp), die Firmware (src/main.cpp: Image-Kopf, Neustart in den Bootloader) und
// der Host-Test (host_test/). Das Sende-Werkzeug (tools/sensact_f103_flash.py) und das ESP32-Gateway
// sprechen dasselbe Protokoll; Aenderungen hier dort nachziehen und PROTOCOL_VERSION erhoehen.
//
// Flash (STM32F103CB, 128 KB in 1-KB-Seiten):
//   0x0800'0000  Bootloader, 16 KB (wird nur per SWD geschrieben)
//   0x0800'4000  Firmware = ein Paket aus Basis-Firmware und einkompilierter Stationskonfiguration
//                (Hausmodell), bis zum Ende des Flashs. Bei +0x100 der Image-Kopf (AppHeader).
//
// Ablauf nach jedem Reset: der Bootloader prueft die Firmware (Kopf, Laenge, CRC32) und wartet
// 1 s auf ein Kommando ueber CAN - 10 s, wenn die Firmware vorher per START_IAP an die Knoten-App
// "Neustart in den Bootloader" verlangt hat (Backup-Register BKP_DR1, s. bkp_stay_in_bootloader),
// unbegrenzt, wenn keine gueltige Firmware da ist. Jedes Kommando an die Station verlaengert das
// Fenster auf 10 s. Danach startet die Firmware.
//
// CAN (29 bit, 125 kbit/s, wie der ganze Bus): die vorhandenen Nachrichtentypen NodeCommand
// (0x02) und NodeCommandAcknowledge (0x03), im alten und im neuen CAN-ID-Schema gleich:
//   Kommando  0x02 | Station << 16 | Kommando << 8      (Host -> Bootloader)
//   Antwort   0x03 | Station << 16 | Kommando << 8      (Bootloader -> Host)
// Station = Nummer der Knoten-App (number der <station> im Hausmodell, LVNG 5, KTCH 6);
// node_unconfigured (0xFF) fuer einen Bootloader ohne gueltige Firmware. Die unteren 10 Bit der
// IDs sind 0, 256, 512 oder 768 (Kommando & 3): eine Station mit altem CAN-ID-Schema, die den
// Nachrichtentyp noch nicht auswertet (ESP32-Basis vor diesem Stand), liest das als Kommando an
// App 0, 256, 512 oder 768 - keine davon ist eine App (0 ist MASTER, 256 ist im Hausmodell
// reserviert, 512 und 768 liegen ueber der hoechsten App-Nummer).
//
//   HELLO  (nur Antwort)   der Bootloader ist gestartet. Daten: s. Info.
//   QUERY  ()              -> QUERY Info. Haelt den Bootloader 10 s fest (jedes Kommando tut das).
//   BEGIN  (Laenge u24, Station u8, CRC32 u32)
//                          Laenge/CRC des ganzen Images (CRC wie im Kopf, s. image_crc). Loescht die
//                          noetigen Seiten (bis ~4 s) -> BEGIN (Status, Seiten u8)
//   DATA   (Folge u8, 7 Byte)
//                          Folge = laufende Framenummer & 0xFF; der letzte Frame darf kuerzer sein.
//                          Nach je window_frames Frames (und nach dem letzten) schreibt der
//                          Bootloader das Fenster in den Flash und antwortet DATA (Status, naechste
//                          Framenummer u16). Der Host sendet das naechste Fenster erst nach der Antwort:
//                          waehrend der Flash schreibt, steht die CPU, und der bxCAN-Empfang ist
//                          nur 3 Frames tief. Fehlt eine Antwort, wiederholt der Host das Fenster ab
//                          der zuletzt bestaetigten Framenummer (Duplikate werden ignoriert, die
//                          Wiederholung eines schon bestaetigten Fensters wird erneut bestaetigt).
//   END    ()              prueft CRC und Image-Kopf -> END (Status, CRC32 u32)
//   START  ()              -> START (Status); danach startet die Firmware (nur wenn gueltig)
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace sensact::boot {

inline constexpr std::uint8_t PROTOCOL_VERSION = 1;
inline constexpr std::uint8_t BOOTLOADER_VERSION = 1;

// ---- Speicher ---------------------------------------------------------------------------------
inline constexpr std::uint32_t flash_base = 0x0800'0000;
inline constexpr std::uint32_t flash_size = 128 * 1024;
inline constexpr std::uint32_t page_size = 1024;
inline constexpr std::uint32_t bootloader_size = 16 * 1024;
inline constexpr std::uint32_t app_base = flash_base + bootloader_size;
inline constexpr std::uint32_t app_max_size = flash_size - bootloader_size;
inline constexpr std::uint32_t app_header_offset = 0x100;  // hinter der Vektortabelle (59 Vektoren, 236 Byte)
inline constexpr std::uint32_t ram_base = 0x2000'0000, ram_size = 20 * 1024;

// ---- Backup-Register (ueberstehen jeden Reset ausser dem Ausfall der Versorgung) ---------------
/// BKP_DR1: die Firmware will in den Bootloader (Kommando START_IAP an die Knoten-App) - 10 s Fenster.
inline constexpr std::uint16_t bkp_stay_in_bootloader = 0xB007;
inline constexpr std::uint32_t window_short_ms = 1'000;
inline constexpr std::uint32_t window_long_ms = 10'000;

// ---- Image-Kopf -------------------------------------------------------------------------------
inline constexpr std::uint32_t app_magic = 0x3150'4153;  // "SAP1"
inline constexpr std::uint16_t header_version = 1;
inline constexpr std::uint16_t board_sensactup02 = 1;  // sensactUp Rev 0.2 (STM32F103CBT6)

/// Bei app_base + app_header_offset, von der Firmware einkompiliert. image_size und image_crc traegt
/// nach dem Bauen tools/f103_image.py ein (im Quelltext 0).
struct AppHeader {
    std::uint32_t magic;
    std::uint16_t version;
    std::uint16_t board;
    std::uint16_t node;           ///< Station (Nummer der Knoten-App)
    std::uint16_t reserved;
    std::uint32_t image_size;     ///< Bytes ab app_base
    std::uint32_t image_crc;      ///< CRC-32 (zlib) ueber das Image, dieses Feld als 0 gerechnet
    std::uint32_t build_time;     ///< Unix-Zeit des Bauens (vom Werkzeug)
    char name[32];                ///< Stationsname, 0-terminiert
};
static_assert(sizeof(AppHeader) == 56);
inline constexpr std::uint32_t image_crc_offset = app_header_offset + offsetof(AppHeader, image_crc);

// ---- CRC-32 (IEEE 802.3, reflektiert, wie zlib.crc32) ------------------------------------------
class Crc32 {
public:
    void update(std::uint8_t b) noexcept {
        crc_ = table[(crc_ ^ b) & 0xFu] ^ (crc_ >> 4);
        crc_ = table[(crc_ ^ (b >> 4)) & 0xFu] ^ (crc_ >> 4);
    }
    void update(const std::uint8_t* p, std::size_t n) noexcept {
        for (std::size_t i = 0; i < n; ++i) update(p[i]);
    }
    [[nodiscard]] std::uint32_t value() const noexcept { return ~crc_; }

private:
    static constexpr std::array<std::uint32_t, 16> table = [] {
        std::array<std::uint32_t, 16> t{};
        for (std::uint32_t i = 0; i < 16; ++i) {
            std::uint32_t c = i;
            for (int k = 0; k < 4; ++k) c = (c & 1u) != 0 ? 0xEDB8'8320u ^ (c >> 1) : c >> 1;
            t[i] = c;
        }
        return t;
    }();
    std::uint32_t crc_ = 0xFFFF'FFFFu;
};

/// CRC des Images wie im Kopf: das Feld image_crc zaehlt als 0.
[[nodiscard]] inline std::uint32_t image_crc(const std::uint8_t* image, std::uint32_t size) noexcept {
    Crc32 c;
    for (std::uint32_t i = 0; i < size; ++i) {
        const bool in_crc_field = i >= image_crc_offset && i < image_crc_offset + 4;
        c.update(in_crc_field ? std::uint8_t{0} : image[i]);
    }
    return c.value();
}

enum class ImageStatus : std::uint8_t { Ok, NoHeader, BadSize, BadVectors, BadCrc, WrongBoard };

[[nodiscard]] inline std::uint32_t le32(const std::uint8_t* p) noexcept {
    return p[0] | (std::uint32_t{p[1]} << 8) | (std::uint32_t{p[2]} << 16) | (std::uint32_t{p[3]} << 24);
}

/// Prueft ein Image ab `image` (im Flash bei app_base; im Host-Test ein Puffer, der als bei app_base
/// liegend gilt). `header` bekommt den Kopf auch bei einem Fehler, soweit er lesbar ist.
[[nodiscard]] inline ImageStatus check_image(const std::uint8_t* image, AppHeader& header) noexcept {
    std::memcpy(&header, image + app_header_offset, sizeof header);
    if (header.magic != app_magic || header.version != header_version) return ImageStatus::NoHeader;
    if (header.image_size < app_header_offset + sizeof(AppHeader) || header.image_size > app_max_size) {
        return ImageStatus::BadSize;
    }
    const std::uint32_t sp = le32(image), entry = le32(image + 4);
    if (sp < ram_base || sp > ram_base + ram_size || (sp & 3u) != 0 || (entry & 1u) == 0 ||
        entry < app_base + app_header_offset || entry >= app_base + header.image_size) {
        return ImageStatus::BadVectors;
    }
    if (header.board != board_sensactup02) return ImageStatus::WrongBoard;
    if (image_crc(image, header.image_size) != header.image_crc) return ImageStatus::BadCrc;
    return ImageStatus::Ok;
}

// ---- CAN --------------------------------------------------------------------------------------
inline constexpr std::uint32_t can_type_mask = 0x1F00'0000;
inline constexpr std::uint32_t can_command = 0x0200'0000;  // NodeCommand
inline constexpr std::uint32_t can_reply = 0x0300'0000;    // NodeCommandAcknowledge
inline constexpr std::uint8_t node_unconfigured = 0xFF;
inline constexpr std::uint32_t window_frames = 16;
inline constexpr std::uint32_t data_per_frame = 7;
inline constexpr std::uint32_t window_bytes = window_frames * data_per_frame;  // 112, gerade: Halbwoerter

/// Die Kommandos (hinter den vorhandenen eNodeCommandType 0..4: NOC, RESET, PAYLOAD, ...).
enum class Cmd : std::uint8_t { Hello = 0x10, Query = 0x11, Begin = 0x12, Data = 0x13, End = 0x14, Start = 0x15 };

enum class Status : std::uint8_t {
    Ok = 0,
    BadState = 1,     ///< Kommando passt nicht zum Ablauf (DATA ohne BEGIN, END vor dem letzten Frame)
    BadSize = 2,      ///< Laenge 0 oder groesser als app_max_size
    WrongNode = 3,    ///< BEGIN fuer eine andere Station
    FlashError = 4,   ///< Loeschen oder Schreiben fehlgeschlagen
    BadLength = 5,    ///< DATA-Frame mit falscher Laenge
    BadCrc = 6,       ///< CRC des geschriebenen Images passt nicht zu BEGIN
    BadImage = 7,     ///< Image-Kopf fehlt/falsch (Board, Station, Vektoren)
    NoApp = 8,        ///< START ohne gueltige Firmware
};

/// Info in HELLO und QUERY.
inline constexpr std::uint8_t info_app_valid = 1u << 0, info_stay_requested = 1u << 1, info_receiving = 1u << 2;

struct Frame {
    std::uint32_t id = 0;
    std::uint8_t length = 0;
    std::array<std::uint8_t, 8> data{};
};

[[nodiscard]] constexpr std::uint32_t command_id(std::uint8_t node, Cmd c) noexcept {
    return can_command | (std::uint32_t{node} << 16) | (std::uint32_t{static_cast<std::uint8_t>(c)} << 8);
}
[[nodiscard]] constexpr std::uint32_t reply_id(std::uint8_t node, Cmd c) noexcept {
    return can_reply | (std::uint32_t{node} << 16) | (std::uint32_t{static_cast<std::uint8_t>(c)} << 8);
}
static_assert((command_id(5, Cmd::Query) & 0x3FFu) == 256 && (reply_id(5, Cmd::Data) & 0x3FFu) == 768);

/// Der Empfangsautomat des Bootloaders. Flash muss bieten:
///   bool erase_page(std::uint32_t address);                               (eine 1-KB-Seite)
///   bool program(std::uint32_t address, const std::uint8_t* p, std::uint32_t n);  (n gerade)
///   const std::uint8_t* image();                                          (Lesezeiger auf app_base)
template <class Flash>
class Session {
public:
    Session(Flash& flash, std::uint8_t node, bool app_valid, std::uint32_t app_crc, bool stay_requested) noexcept
        : flash_(flash), node_(node), app_valid_(app_valid), app_crc_(app_crc), stay_(stay_requested) {}

    [[nodiscard]] std::uint8_t node() const noexcept { return node_; }
    [[nodiscard]] bool app_valid() const noexcept { return app_valid_; }
    /// START angenommen: die Firmware starten (nach dem Senden der Antwort).
    [[nodiscard]] bool start_requested() const noexcept { return start_; }

    [[nodiscard]] Frame hello() const noexcept { return info(Cmd::Hello); }

    /// Ist der Frame ein Kommando an diese Station?
    [[nodiscard]] bool for_us(const Frame& f) const noexcept {
        return (f.id & can_type_mask) == can_command && ((f.id >> 16) & 0xFFu) == node_ && (f.id & 0xFFu) == 0;
    }

    /// Ein Kommando an diese Station (for_us). True: `reply` senden.
    bool handle(const Frame& in, Frame& reply) noexcept {
        const auto cmd = static_cast<Cmd>((in.id >> 8) & 0xFFu);
        switch (cmd) {
            case Cmd::Query: reply = info(Cmd::Query); return true;
            case Cmd::Begin: reply = begin(in); return true;
            case Cmd::Data: return data(in, reply);
            case Cmd::End: reply = end(); return true;
            case Cmd::Start:
                if (!app_valid_ || receiving_) {
                    reply = status(Cmd::Start, app_valid_ ? Status::BadState : Status::NoApp);
                } else {
                    start_ = true;
                    reply = status(Cmd::Start, Status::Ok);
                }
                return true;
            default: return false;
        }
    }

private:
    [[nodiscard]] Frame info(Cmd c) const noexcept {
        Frame f{.id = reply_id(node_, c), .length = 8, .data = {}};
        f.data[0] = PROTOCOL_VERSION;
        f.data[1] = BOOTLOADER_VERSION;
        f.data[2] = static_cast<std::uint8_t>((app_valid_ ? info_app_valid : 0u) | (stay_ ? info_stay_requested : 0u) |
                                              (receiving_ ? info_receiving : 0u));
        f.data[3] = static_cast<std::uint8_t>(board_sensactup02);
        for (int i = 0; i < 4; ++i) f.data[4 + i] = static_cast<std::uint8_t>(app_crc_ >> (8 * i));
        return f;
    }

    [[nodiscard]] Frame status(Cmd c, Status s) const noexcept {
        Frame f{.id = reply_id(node_, c), .length = 1, .data = {}};
        f.data[0] = static_cast<std::uint8_t>(s);
        return f;
    }

    Frame begin(const Frame& in) noexcept {
        if (in.length != 8) return status(Cmd::Begin, Status::BadLength);
        const std::uint32_t size = in.data[0] | (std::uint32_t{in.data[1]} << 8) | (std::uint32_t{in.data[2]} << 16);
        const std::uint8_t node = in.data[3];
        if (node != node_ && node_ != node_unconfigured) return status(Cmd::Begin, Status::WrongNode);
        if (size < app_header_offset + sizeof(AppHeader) || size > app_max_size) return status(Cmd::Begin, Status::BadSize);
        // ab hier ist die alte Firmware weg: bis zu einem erfolgreichen END bleibt der Bootloader aktiv
        app_valid_ = false;
        receiving_ = false;
        image_node_ = node;
        size_ = size;
        crc_ = le32(&in.data[4]);
        const std::uint32_t pages = (size + page_size - 1) / page_size;
        for (std::uint32_t p = 0; p < pages; ++p) {
            if (!flash_.erase_page(app_base + p * page_size)) return status(Cmd::Begin, Status::FlashError);
        }
        received_ = 0;
        frames_ = 0;
        fill_ = 0;
        receiving_ = true;
        Frame f = status(Cmd::Begin, Status::Ok);
        f.length = 2;
        f.data[1] = static_cast<std::uint8_t>(pages);
        return f;
    }

    bool data(const Frame& in, Frame& reply) noexcept {
        if (!receiving_) {
            reply = status(Cmd::Data, Status::BadState);
            return true;
        }
        if (in.length < 2) return false;
        const std::uint8_t seq = in.data[0];
        if (seq != static_cast<std::uint8_t>(frames_)) {
            // Wiederholung des zuletzt bestaetigten Fensters (die Antwort ging verloren): erneut bestaetigen
            if (fill_ == 0 && frames_ != 0 && seq == static_cast<std::uint8_t>(frames_ - 1)) {
                reply = data_ack(Status::Ok);
                return true;
            }
            return false;  // Duplikat oder Luecke: der Host wiederholt ab der letzten Bestaetigung
        }
        const std::uint32_t remaining = size_ - received_;
        const std::uint32_t n = remaining < data_per_frame ? remaining : data_per_frame;
        if (n == 0 || in.length != n + 1) {
            reply = data_ack(Status::BadLength);
            return true;
        }
        std::memcpy(&window_[fill_], &in.data[1], n);
        fill_ += n;
        received_ += n;
        ++frames_;
        if (frames_ % window_frames != 0 && received_ != size_) return false;
        // Fenster voll oder Image komplett: schreiben (auf Halbwoerter mit 0xFF aufgefuellt)
        std::uint32_t len = fill_;
        if ((len & 1u) != 0) window_[len++] = 0xFF;
        const std::uint32_t at = app_base + received_ - fill_;
        fill_ = 0;
        if (!flash_.program(at, window_.data(), len)) {
            receiving_ = false;
            reply = data_ack(Status::FlashError);
            return true;
        }
        reply = data_ack(Status::Ok);
        return true;
    }

    [[nodiscard]] Frame data_ack(Status s) const noexcept {
        Frame f = status(Cmd::Data, s);
        f.length = 3;
        f.data[1] = static_cast<std::uint8_t>(frames_);
        f.data[2] = static_cast<std::uint8_t>(frames_ >> 8);
        return f;
    }

    Frame end() noexcept {
        if (!receiving_ || received_ != size_) return status(Cmd::End, Status::BadState);
        receiving_ = false;
        const std::uint8_t* image = flash_.image();
        const std::uint32_t crc = image_crc(image, size_);
        Frame f = status(Cmd::End, Status::Ok);
        f.length = 5;
        for (int i = 0; i < 4; ++i) f.data[1 + i] = static_cast<std::uint8_t>(crc >> (8 * i));
        if (crc != crc_) {
            f.data[0] = static_cast<std::uint8_t>(Status::BadCrc);
            return f;
        }
        AppHeader h{};
        if (check_image(image, h) != ImageStatus::Ok || h.image_size != size_ || h.node != image_node_) {
            f.data[0] = static_cast<std::uint8_t>(Status::BadImage);
            return f;
        }
        app_valid_ = true;
        app_crc_ = crc;
        node_ = image_node_;  // ein bisher unkonfigurierter Knoten heisst ab jetzt wie seine Firmware
        f.id = reply_id(node_, Cmd::End);
        return f;
    }

    Flash& flash_;
    std::uint8_t node_;
    bool app_valid_;
    std::uint32_t app_crc_;
    bool stay_;
    bool start_ = false;
    bool receiving_ = false;
    std::uint8_t image_node_ = 0;
    std::uint32_t size_ = 0, crc_ = 0, received_ = 0, frames_ = 0, fill_ = 0;
    std::array<std::uint8_t, window_bytes + 1> window_{};
};

}  // namespace sensact::boot
