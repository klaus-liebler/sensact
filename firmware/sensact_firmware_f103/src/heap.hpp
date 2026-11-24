#pragma once
// Speicher fuer die App-Fabrik. rtoos hat zur Laufzeit keinen Heap; die gemeinsamen Sensact-Apps
// (sensact-applicationmodel, auch auf dem ESP32) legen ihre Objekte und Listen aber beim Start mit
// new/std::vector an. Hier bekommen sie einen festen Bereich (operator new/delete, First Fit mit
// Zusammenfassen freier Nachbarn), der nach dem Start versiegelt wird: danach bricht jedes new den
// Knoten ab (Contract -> Watchdog-Reset) - so faellt eine Allokation im Betrieb sofort auf, statt den
// Speicher langsam zu zerstueckeln.
#include <cstddef>

namespace heap {

/// Ab jetzt keine Allokation mehr (nach LoadStationConfig() und cApplicationHost::Setup()).
void seal() noexcept;
[[nodiscard]] std::size_t capacity() noexcept;
[[nodiscard]] std::size_t in_use() noexcept;
[[nodiscard]] std::size_t peak() noexcept;

}  // namespace heap
