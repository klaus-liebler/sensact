// Was Bootloader und Firmware am STM32F103 gemeinsam brauchen: das Backup-Register BKP_DR1 (Wunsch
// "im Bootloader bleiben" ueber einen Reset hinweg) und den Software-Reset. RM0008 rev 21:
// 5.4.1 PWR_CR (DBP), 6.4.1 BKP_DRx, 7.3.8 RCC_APB1ENR (PWREN, BKPEN); ARMv7-M AIRCR.SYSRESETREQ.
//
// Die Backup-Register stehen in der Backup-Domaene: ein System-Reset (Watchdog, SYSRESETREQ, NRST)
// laesst sie stehen, ein Ausfall der Versorgung (VBAT haengt auf der sensactUp an VDD) setzt sie auf 0.
#pragma once

#include <cstdint>

#include "boot_protocol.hpp"
#include "rtos/port/cortex_m/registers.hpp"
#include "rtos/port/stm32f1/chip.hpp"
#include "rtos/port/stm32f1/rcc.hpp"

namespace sensact::boot::hw {

namespace f1 = rtos::port::stm32f1;
namespace scb = rtos::port::cortex_m::scb;

using PWR_CR = f1::PReg<0x0000'7000 + 0x00>;
using BKP_DR1 = f1::PReg<0x0000'6C00 + 0x04>;
inline constexpr std::uint32_t pwr_cr_dbp = 1u << 8;

/// Takt fuer PWR und BKP, Schreibschutz der Backup-Domaene aus (DBP).
inline void backup_access_on() noexcept {
    f1::rcc::enable(f1::rcc::gate::pwr);
    f1::rcc::enable(f1::rcc::gate::bkp);
    PWR_CR::set(pwr_cr_dbp);
}

/// Wieder wie nach dem Reset (vor dem Sprung in die Firmware).
inline void backup_access_off() noexcept {
    PWR_CR::clear(pwr_cr_dbp);
    f1::rcc::disable(f1::rcc::gate::bkp);
    f1::rcc::disable(f1::rcc::gate::pwr);
}

/// BKP_DR1 lesen und loeschen (Bootloader). Der Zugriff bleibt an.
[[nodiscard]] inline std::uint16_t take_boot_request() noexcept {
    backup_access_on();
    const auto v = static_cast<std::uint16_t>(BKP_DR1::read() & 0xFFFFu);
    BKP_DR1::write(0);
    return v;
}

/// System-Reset (alle Peripherie zurueck, die Backup-Domaene bleibt).
[[noreturn]] inline void system_reset() noexcept {
    __asm volatile("dsb" ::: "memory");
    scb::AIRCR::write(scb::aircr_vectkey | (scb::AIRCR::read() & scb::aircr_prigroup_mask) | scb::aircr_sysresetreq);
    __asm volatile("dsb" ::: "memory");
    for (;;) {
    }
}

/// Neustart (Firmware): mit `to_bootloader` wartet der Bootloader danach 10 s statt 1 s auf ein Update.
[[noreturn]] inline void restart(bool to_bootloader) noexcept {
    backup_access_on();
    BKP_DR1::write(to_bootloader ? bkp_stay_in_bootloader : 0u);
    system_reset();
}

}  // namespace sensact::boot::hw
