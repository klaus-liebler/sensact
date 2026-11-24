#pragma once
// Der Kernel der Station (rtoos auf dem Cortex-M3) - fuer die Teile, die ihn brauchen (Log-Sperre).
#include <cstddef>

#include "rtos/port/cm3/port.hpp"
#include "rtos/rtos.hpp"

namespace firmware {

struct KernelConfig : rtos::DefaultConfig {
    static constexpr std::size_t max_threads = 2;  // App-Thread, IO-Thread (PCA9685)
    static constexpr std::size_t priority_levels = 8;
    static constexpr std::size_t notify_slots = 1;
    static constexpr std::size_t timer_wheel_slots = 8;
    static constexpr std::size_t idle_stack_bytes = 256;
};

using Port = rtos::port::cm3::Cm3Port;
using Kernel = rtos::Kernel<Port, KernelConfig>;

}  // namespace firmware

extern firmware::Kernel kernel;
