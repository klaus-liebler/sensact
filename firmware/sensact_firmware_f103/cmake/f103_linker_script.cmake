# Linker-Skript fuer Bootloader bzw. Firmware aus dem des Chips (rtoos port/stm32f103/stm32f103xb.ld),
# damit Abschnitte, Pruefungen und Symbole dort gepflegt bleiben. Aufteilung: boot/boot_protocol.hpp.
#
#   sensact_f103_linker_script(<Ausgabe> BOOTLOADER | FIRMWARE)
# BOOTLOADER: Flash 0x0800'0000, 16 KB. FIRMWARE: Flash ab 0x0800'4000 (112 KB), der Image-Kopf
# (Abschnitt .app_header) fest bei +0x100 hinter der Vektortabelle.
function(sensact_f103_linker_script out kind)
    set(_src "${RTOOS_DIR}/port/stm32f103/stm32f103xb.ld")
    file(READ "${_src}" _ld)
    set(_memory "FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 128K")
    string(FIND "${_ld}" "${_memory}" _at)
    if(_at EQUAL -1)
        message(FATAL_ERROR "${_src}: MEMORY-Zeile '${_memory}' nicht gefunden - cmake/f103_linker_script.cmake anpassen")
    endif()
    if(kind STREQUAL "BOOTLOADER")
        string(REPLACE "${_memory}" "FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 16K" _ld "${_ld}")
    elseif(kind STREQUAL "FIRMWARE")
        string(REPLACE "${_memory}" "FLASH   (rx)  : ORIGIN = 0x08004000, LENGTH = 112K" _ld "${_ld}")
        set(_vectors "  .isr_vector : ALIGN(256)\n  {\n    KEEP(*(.isr_vector))\n  } > FLASH\n")
        string(FIND "${_ld}" "${_vectors}" _at)
        if(_at EQUAL -1)
            message(FATAL_ERROR "${_src}: Abschnitt .isr_vector nicht wie erwartet - cmake/f103_linker_script.cmake anpassen")
        endif()
        string(REPLACE "${_vectors}" "${_vectors}
  /* sensact: Image-Kopf fuer den Bootloader (boot/boot_protocol.hpp, AppHeader) */
  .app_header ORIGIN(FLASH) + 0x100 :
  {
    KEEP(*(.app_header))
  } > FLASH
  ASSERT(ADDR(.isr_vector) == ORIGIN(FLASH), \"vector table must start the image\")
  ASSERT(SIZEOF(.app_header) == 56, \"exactly one AppHeader in .app_header\")
" _ld "${_ld}")
    else()
        message(FATAL_ERROR "sensact_f103_linker_script: BOOTLOADER oder FIRMWARE")
    endif()
    file(CONFIGURE OUTPUT "${out}" CONTENT "/* erzeugt aus ${_src} (cmake/f103_linker_script.cmake) */\n${_ld}" @ONLY)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_src}")
endfunction()
