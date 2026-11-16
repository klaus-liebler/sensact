#pragma once
// Host-Ersatz fuer espidf-components/common/include/common-esp32.hh. Von den Apps wird daraus nur
// FLASH_FILE gebraucht (sound.cc: eingebettete MP3-Dateien). Auf dem Host gibt es keine vom
// Linker eingebetteten Binaerdateien -- stattdessen ein kleiner, pro Datei eindeutiger
// Platzhalter-Puffer, damit Tests pruefen koennen, WELCHE Datei abgespielt wurde.
#include <cstddef>
#include <cstdint>

#define FLASH_FILE(x)                                                   \
    static const uint8_t x##_start[] = {'I', 'D', '3', #x[0], #x[1]};   \
    static const uint8_t *const x##_end = x##_start + sizeof(x##_start); \
    static const size_t x##_size = sizeof(x##_start);
