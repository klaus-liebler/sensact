// Der Empfangsautomat des Bootloaders als Kommandozeilenprogramm, fuer den Test des Sende-Werkzeugs
// (test_sensact_f103_flash.py): je Zeile auf stdin ein CAN-Frame "ID DATEN" (hex), je Antwort eine
// Zeile auf stdout. Flash wie im Host-Test (nur geloeschte Zellen beschreibbar).
//
//   boot_sim <Station> [app_valid 0/1]
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "boot_protocol.hpp"

using namespace sensact::boot;

namespace {
struct SimFlash {
    std::vector<std::uint8_t> mem = std::vector<std::uint8_t>(app_max_size, 0x00);
    bool erase_page(std::uint32_t a) {
        std::memset(&mem[a - app_base], 0xFF, page_size);
        return true;
    }
    bool program(std::uint32_t a, const std::uint8_t* p, std::uint32_t n) {
        for (std::uint32_t i = 0; i < n; ++i) {
            if (mem[a - app_base + i] != 0xFF) return false;
            mem[a - app_base + i] = p[i];
        }
        return true;
    }
    const std::uint8_t* image() { return mem.data(); }
};

void print(const Frame& f) {
    std::printf("%08X ", static_cast<unsigned>(f.id));
    for (int i = 0; i < f.length; ++i) std::printf("%02X", f.data[i]);
    std::printf("\n");
    std::fflush(stdout);
}
}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) return 2;
    SimFlash flash;
    Session s(flash, static_cast<std::uint8_t>(std::atoi(argv[1])), argc > 2 && argv[2][0] == '1', 0, true);
    print(s.hello());
    std::string id_hex, data_hex;
    std::string line;
    while (std::getline(std::cin, line)) {
        const auto space = line.find(' ');
        Frame f;
        f.id = static_cast<std::uint32_t>(std::stoul(line.substr(0, space), nullptr, 16));
        const std::string d = space == std::string::npos ? "" : line.substr(space + 1);
        f.length = static_cast<std::uint8_t>(d.size() / 2);
        for (int i = 0; i < f.length; ++i) f.data[i] = static_cast<std::uint8_t>(std::stoul(d.substr(2 * i, 2), nullptr, 16));
        if (!s.for_us(f)) continue;
        Frame r;
        if (s.handle(f, r)) print(r);
        if (s.start_requested()) {
            std::fprintf(stderr, "STARTED node=%u\n", s.node());
            return 0;
        }
    }
    return 1;
}
