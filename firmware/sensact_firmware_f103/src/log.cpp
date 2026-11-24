// Log der gemeinsamen Sensact-Apps (ESP_LOGx, stubs/esp_log.h) auf die Konsole USART1 (Stecker P4,
// 115200 Baud): "I (12345) TAG: Text". Nie blockierend: ist der Sendepuffer voll, fehlt der Rest.
// Zwei Threads loggen in denselben Puffer (ein Erzeuger je Ring): Formatieren und Einreihen unter
// der Kernel-Sperre - ein paar zehn Mikrosekunden.
#include <cstdarg>
#include <cstdio>
#include <span>

#include "board/sensactup02.hpp"
#include "firmware.hpp"
#include "stubs/esp_log.h"

extern "C" void sensact_log(char level, const char* tag, const char* format, ...) {
    char line[160];
    firmware::Kernel::Lock lock;
    int n = std::snprintf(line, sizeof line, "%c (%lu) %s: ", level, static_cast<unsigned long>(kernel.now()), tag);
    if (n < 0) return;
    va_list args;
    va_start(args, format);
    const int m = std::vsnprintf(line + n, sizeof line - static_cast<std::size_t>(n), format, args);
    va_end(args);
    n = m < 0 ? n : n + m;
    if (n > static_cast<int>(sizeof line) - 3) n = sizeof line - 3;
    line[n++] = '\r';
    line[n++] = '\n';
    (void)board::console.write(std::as_bytes(std::span(line, static_cast<std::size_t>(n))));
}
