// Flash-Programmierung des STM32F103 (FPEC, RM0008 rev 21 3.3 / PM0075): Seite loeschen, Halbwoerter
// schreiben. Nur der Bootloader schreibt den Flash; er laeuft selbst aus dem Flash, die CPU steht
// waehrend jedes Schreib- und Loeschvorgangs (deshalb wartet der Host auf jede Antwort, s.
// boot_protocol.hpp). Der FPEC braucht den HSI (PM0075 2.3): er bleibt an.
#pragma once

#include <cstdint>

#include "boot_protocol.hpp"
#include "rtos/port/stm32f1/chip.hpp"
#include "rtos/port/stm32f1/rcc.hpp"

namespace bootloader {

namespace f1 = rtos::port::stm32f1;

template <class Watchdog>
class F1Flash {
public:
    /// Eine 1-KB-Seite der Firmware loeschen (20..40 ms) und pruefen.
    bool erase_page(std::uint32_t address) noexcept {
        if (address < sensact::boot::app_base || address >= sensact::boot::flash_base + sensact::boot::flash_size) {
            return false;  // der Bootloader loescht sich nie selbst
        }
        Watchdog::kick();
        unlock();
        CR::set(cr_per);
        AR::write(address);
        CR::set(cr_strt);
        const bool ok = wait_done();
        CR::clear(cr_per);
        lock();
        Watchdog::kick();
        if (!ok) return false;
        const auto* p = reinterpret_cast<const volatile std::uint32_t*>(address);
        for (std::uint32_t i = 0; i < sensact::boot::page_size / 4; ++i) {
            if (p[i] != 0xFFFF'FFFFu) return false;
        }
        return true;
    }

    /// n Bytes (gerade) ab address (gerade) schreiben und pruefen.
    bool program(std::uint32_t address, const std::uint8_t* data, std::uint32_t n) noexcept {
        if ((address & 1u) != 0 || (n & 1u) != 0 || address < sensact::boot::app_base ||
            address + n > sensact::boot::flash_base + sensact::boot::flash_size) {
            return false;
        }
        Watchdog::kick();
        unlock();
        CR::set(cr_pg);
        bool ok = true;
        for (std::uint32_t i = 0; i < n && ok; i += 2) {
            auto* cell = reinterpret_cast<volatile std::uint16_t*>(address + i);
            const auto v = static_cast<std::uint16_t>(data[i] | (data[i + 1] << 8));
            *cell = v;
            ok = wait_done() && *cell == v;
        }
        CR::clear(cr_pg);
        lock();
        return ok;
    }

    [[nodiscard]] const std::uint8_t* image() const noexcept {
        return reinterpret_cast<const std::uint8_t*>(sensact::boot::app_base);
    }

private:
    using KEYR = f1::PReg<0x0002'2000 + 0x04>;
    using SR = f1::PReg<0x0002'2000 + 0x0C>;
    using CR = f1::PReg<0x0002'2000 + 0x10>;
    using AR = f1::PReg<0x0002'2000 + 0x14>;
    static constexpr std::uint32_t sr_bsy = 1u << 0, sr_pgerr = 1u << 2, sr_wrprterr = 1u << 4, sr_eop = 1u << 5;
    static constexpr std::uint32_t cr_pg = 1u << 0, cr_per = 1u << 1, cr_strt = 1u << 6, cr_lock = 1u << 7;

    static void unlock() noexcept {
        f1::rcc::CR::set(f1::rcc::cr_hsion);
        while ((f1::rcc::CR::read() & f1::rcc::cr_hsirdy) == 0) {
        }
        while ((SR::read() & sr_bsy) != 0) {
        }
        if ((CR::read() & cr_lock) != 0) {
            KEYR::write(0x4567'0123u);
            KEYR::write(0xCDEF'89ABu);
        }
        SR::write(sr_eop | sr_pgerr | sr_wrprterr);  // Flags loeschen (1 schreiben)
    }
    static void lock() noexcept { CR::set(cr_lock); }

    /// Ende des Vorgangs; false bei PGERR (Zelle nicht geloescht) oder WRPRTERR (Schreibschutz).
    static bool wait_done() noexcept {
        while ((SR::read() & sr_bsy) != 0) {
        }
        const std::uint32_t sr = SR::read();
        SR::write(sr_eop | sr_pgerr | sr_wrprterr);
        return (sr & (sr_pgerr | sr_wrprterr)) == 0;
    }
};

}  // namespace bootloader
