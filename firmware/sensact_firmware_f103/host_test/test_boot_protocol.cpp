// Host-Tests fuer boot/boot_protocol.hpp: Empfangsautomat des Bootloaders gegen einen Flash, der sich
// wie der des F103 verhaelt (Halbwort nur auf geloeschte Zellen, Loeschen seitenweise), und ein
// Sender wie tools/sensact_f103_flash.py mit verlorenen Frames und Antworten.
#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>
#include <random>
#include <vector>

#include "boot_protocol.hpp"

using namespace sensact::boot;

namespace {

class FakeFlash {
public:
    FakeFlash() : mem(app_max_size, 0x00) {}  // nicht geloescht: Schreiben ohne Loeschen faellt auf

    bool erase_page(std::uint32_t address) {
        if (address < app_base || (address - app_base) % page_size != 0 || address - app_base >= app_max_size) return false;
        std::memset(&mem[address - app_base], 0xFF, page_size);
        ++erases;
        return !fail_erase;
    }
    bool program(std::uint32_t address, const std::uint8_t* p, std::uint32_t n) {
        if ((address & 1u) != 0 || (n & 1u) != 0 || address < app_base || address - app_base + n > app_max_size) return false;
        for (std::uint32_t i = 0; i < n; i += 2) {
            std::uint8_t* cell = &mem[address - app_base + i];
            if (cell[0] != 0xFF || cell[1] != 0xFF) return false;  // F1: PGERR, Halbwort nicht geloescht
            cell[0] = p[i];
            cell[1] = p[i + 1];
        }
        ++programs;
        return !fail_program;
    }
    const std::uint8_t* image() { return mem.data(); }

    std::vector<std::uint8_t> mem;
    int erases = 0, programs = 0;
    bool fail_erase = false, fail_program = false;
};

std::vector<std::uint8_t> make_image(std::uint32_t size, std::uint16_t node, std::uint32_t seed = 1) {
    std::vector<std::uint8_t> img(size);
    std::mt19937 rng(seed);
    for (auto& b : img) b = static_cast<std::uint8_t>(rng());
    auto put32 = [&](std::uint32_t at, std::uint32_t v) {
        for (int i = 0; i < 4; ++i) img[at + i] = static_cast<std::uint8_t>(v >> (8 * i));
    };
    put32(0, ram_base + ram_size);         // MSP
    put32(4, app_base + 0x201);            // Reset_Handler (Thumb)
    AppHeader h{};
    h.magic = app_magic;
    h.version = header_version;
    h.board = board_sensactup02;
    h.node = node;
    h.image_size = size;
    h.image_crc = 0;
    h.build_time = 1'764'000'000;
    std::strncpy(h.name, "SNSCT_L1_LVNG_UP", sizeof h.name);
    std::memcpy(&img[app_header_offset], &h, sizeof h);
    const std::uint32_t crc = image_crc(img.data(), size);
    put32(image_crc_offset, crc);
    return img;
}

Frame cmd(std::uint8_t node, Cmd c, std::initializer_list<std::uint8_t> data = {}) {
    Frame f{.id = command_id(node, c), .length = static_cast<std::uint8_t>(data.size()), .data = {}};
    std::size_t i = 0;
    for (auto b : data) f.data[i++] = b;
    return f;
}

Frame begin_frame(std::uint8_t node, std::uint32_t size, std::uint8_t image_node, std::uint32_t crc) {
    return cmd(node, Cmd::Begin, {static_cast<std::uint8_t>(size), static_cast<std::uint8_t>(size >> 8),
                                  static_cast<std::uint8_t>(size >> 16), image_node, static_cast<std::uint8_t>(crc),
                                  static_cast<std::uint8_t>(crc >> 8), static_cast<std::uint8_t>(crc >> 16),
                                  static_cast<std::uint8_t>(crc >> 24)});
}

Frame data_frame(std::uint8_t node, const std::vector<std::uint8_t>& img, std::uint32_t index) {
    Frame f{.id = command_id(node, Cmd::Data), .length = 0, .data = {}};
    const std::uint32_t at = index * data_per_frame;
    const std::uint32_t n = std::min<std::uint32_t>(data_per_frame, static_cast<std::uint32_t>(img.size()) - at);
    f.data[0] = static_cast<std::uint8_t>(index);
    std::memcpy(&f.data[1], &img[at], n);
    f.length = static_cast<std::uint8_t>(n + 1);
    return f;
}

Status status_of(const Frame& f) { return static_cast<Status>(f.data[0]); }

/// Der Sender wie tools/sensact_f103_flash.py: Fenster zu window_frames Frames, auf die Antwort warten,
/// bei fehlender Antwort ab der letzten Bestaetigung wiederholen. drop_to/drop_from: true = Frame verloren.
struct Link {
    Session<FakeFlash>& s;
    std::function<bool(const Frame&)> drop_to = [](const Frame&) { return false; };
    std::function<bool(const Frame&)> drop_from = [](const Frame&) { return false; };
    int sent = 0;

    std::deque<Frame> exchange(const Frame& f) {
        ++sent;
        std::deque<Frame> replies;
        if (drop_to(f) || !s.for_us(f)) return replies;
        Frame r;
        if (s.handle(f, r) && !drop_from(r)) replies.push_back(r);
        return replies;
    }

    /// Ganzes Image; Ergebnis: Status von END (oder des ersten Fehlers).
    Status flash(std::uint8_t node, const std::vector<std::uint8_t>& img, std::uint8_t image_node) {
        const std::uint32_t crc = image_crc(img.data(), static_cast<std::uint32_t>(img.size()));
        auto r = exchange(begin_frame(node, static_cast<std::uint32_t>(img.size()), image_node, crc));
        if (r.empty()) return Status::BadState;
        if (status_of(r.front()) != Status::Ok) return status_of(r.front());
        const std::uint32_t total = static_cast<std::uint32_t>((img.size() + data_per_frame - 1) / data_per_frame);
        std::uint32_t acked = 0;
        int retries = 0;
        while (acked < total) {
            const std::uint32_t end = std::min(total, acked + window_frames);
            std::deque<Frame> got;
            for (std::uint32_t i = acked; i < end; ++i) {
                for (auto& x : exchange(data_frame(node, img, i))) got.push_back(x);
            }
            if (got.empty()) {
                if (++retries > 20) return Status::BadState;
                continue;  // Zeitueberschreitung: Fenster wiederholen
            }
            const Frame& a = got.back();
            if (status_of(a) != Status::Ok) return status_of(a);
            acked = a.data[1] | (std::uint32_t{a.data[2]} << 8);
            retries = 0;
        }
        r = exchange(cmd(node, Cmd::End));
        if (r.empty()) return Status::BadState;
        return status_of(r.front());
    }
};

}  // namespace

TEST(Crc32, MatchesZlib) {
    Crc32 c;
    const char* s = "123456789";
    c.update(reinterpret_cast<const std::uint8_t*>(s), 9);
    EXPECT_EQ(c.value(), 0xCBF4'3926u);  // zlib.crc32(b"123456789")
}

TEST(Image, ValidImagePasses) {
    auto img = make_image(5000, 5);
    AppHeader h{};
    EXPECT_EQ(check_image(img.data(), h), ImageStatus::Ok);
    EXPECT_EQ(h.node, 5);
    EXPECT_STREQ(h.name, "SNSCT_L1_LVNG_UP");
}

TEST(Image, CorruptionIsDetected) {
    auto img = make_image(5000, 5);
    img[4000] ^= 1;
    AppHeader h{};
    EXPECT_EQ(check_image(img.data(), h), ImageStatus::BadCrc);
}

TEST(Image, ErasedFlashHasNoHeader) {
    std::vector<std::uint8_t> erased(app_max_size, 0xFF);
    AppHeader h{};
    EXPECT_EQ(check_image(erased.data(), h), ImageStatus::NoHeader);
}

TEST(Image, VectorsOutsideTheImageAreRejected) {
    auto img = make_image(5000, 5);
    img[4] = 0x00;  // Reset-Vektor gerade (kein Thumb) bzw. vor dem Kopf
    img[5] = 0x00;
    AppHeader h{};
    EXPECT_EQ(check_image(img.data(), h), ImageStatus::BadVectors);
}

TEST(Session, CommandsForOtherStationsAreIgnored) {
    FakeFlash flash;
    Session s(flash, 5, true, 0, false);
    EXPECT_TRUE(s.for_us(cmd(5, Cmd::Query)));
    EXPECT_FALSE(s.for_us(cmd(6, Cmd::Query)));
    Frame app_command{.id = 5, .length = 1, .data = {1}};  // altes Schema: START_IAP an App 5
    EXPECT_FALSE(s.for_us(app_command));
    Frame reply{.id = reply_id(5, Cmd::Query), .length = 0, .data = {}};
    EXPECT_FALSE(s.for_us(reply));
}

TEST(Session, QueryReportsTheFirmware) {
    FakeFlash flash;
    Session s(flash, 5, true, 0x1234'5678, true);
    Frame r;
    ASSERT_TRUE(s.handle(cmd(5, Cmd::Query), r));
    EXPECT_EQ(r.id, 0x0305'1100u);
    EXPECT_EQ(r.data[0], PROTOCOL_VERSION);
    EXPECT_EQ(r.data[2], info_app_valid | info_stay_requested);
    EXPECT_EQ(le32(&r.data[4]), 0x1234'5678u);
    EXPECT_EQ(s.hello().id, 0x0305'1000u);
}

TEST(Session, FlashesACompleteImage) {
    FakeFlash flash;
    Session s(flash, 5, true, 0, false);
    Link link{s};
    auto img = make_image(54'321, 5);
    EXPECT_EQ(link.flash(5, img, 5), Status::Ok);
    EXPECT_TRUE(s.app_valid());
    EXPECT_EQ(flash.erases, 54);  // nur die noetigen Seiten
    EXPECT_EQ(0, std::memcmp(flash.mem.data(), img.data(), img.size()));
    AppHeader h{};
    EXPECT_EQ(check_image(flash.image(), h), ImageStatus::Ok);

    Frame r;
    ASSERT_TRUE(s.handle(cmd(5, Cmd::Start), r));
    EXPECT_EQ(status_of(r), Status::Ok);
    EXPECT_TRUE(s.start_requested());
}

TEST(Session, LostDataFramesAreRepeated) {
    FakeFlash flash;
    Session s(flash, 5, false, 0, false);
    Link link{s};
    int n = 0;
    link.drop_to = [&](const Frame& f) { return ((f.id >> 8) & 0xFF) == 0x13 && ++n % 37 == 0; };
    auto img = make_image(20'000, 5, 7);
    EXPECT_EQ(link.flash(5, img, 5), Status::Ok);
    EXPECT_EQ(0, std::memcmp(flash.mem.data(), img.data(), img.size()));
}

TEST(Session, LostAcknowledgementsAreRepeated) {
    // Eine verlorene DATA-Antwort: der Host wiederholt das schon geschriebene Fenster; der Bootloader
    // darf es nicht noch einmal schreiben (der Flash nimmt kein zweites Schreiben ohne Loeschen an)
    FakeFlash flash;
    Session s(flash, 5, false, 0, false);
    Link link{s};
    int n = 0;
    link.drop_from = [&](const Frame& f) { return ((f.id >> 8) & 0xFF) == 0x13 && ++n % 5 == 0; };
    auto img = make_image(20'003, 5, 9);  // ungerade Laenge: letzter Frame kurz, Fenster aufgefuellt
    EXPECT_EQ(link.flash(5, img, 5), Status::Ok);
    EXPECT_EQ(0, std::memcmp(flash.mem.data(), img.data(), img.size()));
}

TEST(Session, WrongStationIsRefusedBeforeErasing) {
    FakeFlash flash;
    Session s(flash, 5, true, 0, false);
    Link link{s};
    auto img = make_image(5000, 6);  // Paket der Kueche an das Wohnzimmer
    EXPECT_EQ(link.flash(5, img, 6), Status::WrongNode);
    EXPECT_EQ(flash.erases, 0);
    EXPECT_TRUE(s.app_valid());
}

TEST(Session, HeaderMustNameTheStationOfBegin) {
    FakeFlash flash;
    Session s(flash, 5, true, 0, false);
    Link link{s};
    auto img = make_image(5000, 6);  // Kopf sagt 6, BEGIN behauptet 5
    EXPECT_EQ(link.flash(5, img, 5), Status::BadImage);
    EXPECT_FALSE(s.app_valid());
    Frame r;
    ASSERT_TRUE(s.handle(cmd(5, Cmd::Start), r));
    EXPECT_EQ(status_of(r), Status::NoApp);
    EXPECT_FALSE(s.start_requested());
}

TEST(Session, CrcMismatchLeavesNoValidFirmware) {
    FakeFlash flash;
    Session s(flash, 5, true, 0, false);
    Link link{s};
    auto img = make_image(5000, 5);
    Frame r;
    ASSERT_TRUE(s.handle(begin_frame(5, 5000, 5, 0xDEAD'BEEF), r));
    ASSERT_EQ(status_of(r), Status::Ok);
    for (std::uint32_t i = 0; i < (5000 + 6) / 7; ++i) (void)link.exchange(data_frame(5, img, i));
    ASSERT_TRUE(s.handle(cmd(5, Cmd::End), r));
    EXPECT_EQ(status_of(r), Status::BadCrc);
    EXPECT_FALSE(s.app_valid());
}

TEST(Session, UnconfiguredNodeTakesTheStationOfItsFirmware) {
    FakeFlash flash;
    Session s(flash, node_unconfigured, false, 0, false);
    Link link{s};
    auto img = make_image(3000, 6);
    EXPECT_EQ(link.flash(node_unconfigured, img, 6), Status::Ok);
    EXPECT_EQ(s.node(), 6);
    EXPECT_TRUE(s.for_us(cmd(6, Cmd::Start)));
}

TEST(Session, RejectsBadSizesAndOrder) {
    FakeFlash flash;
    Session s(flash, 5, true, 0, false);
    Frame r;
    ASSERT_TRUE(s.handle(data_frame(5, make_image(1000, 5), 0), r));
    EXPECT_EQ(status_of(r), Status::BadState);
    ASSERT_TRUE(s.handle(begin_frame(5, app_max_size + 2, 5, 0), r));
    EXPECT_EQ(status_of(r), Status::BadSize);
    ASSERT_TRUE(s.handle(begin_frame(5, 16, 5, 0), r));
    EXPECT_EQ(status_of(r), Status::BadSize);
    ASSERT_TRUE(s.handle(cmd(5, Cmd::End), r));
    EXPECT_EQ(status_of(r), Status::BadState);
    EXPECT_TRUE(s.app_valid());  // nichts geloescht
}

TEST(Session, FlashErrorIsReported) {
    FakeFlash flash;
    flash.fail_program = true;
    Session s(flash, 5, true, 0, false);
    Link link{s};
    EXPECT_EQ(link.flash(5, make_image(3000, 5), 5), Status::FlashError);
    EXPECT_FALSE(s.app_valid());
}
