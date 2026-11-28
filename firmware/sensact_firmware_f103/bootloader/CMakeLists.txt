# CAN-Bootloader der sensactUp-Stationen (STM32F103CB): 0x0800'0000..0x0800'3FFF. Wird einmal per SWD
# geschrieben; danach kommt jede Firmware ueber den CAN-Bus (boot/boot_protocol.hpp, tools/sensact_f103_flash.py).
#
#   cmake --preset bootloader && cmake --build --preset bootloader   -> build/bootloader/sensact_f103_boot.bin
#
# Eigenes CMake-Projekt (eigener Build-Baum): der Linker-Skript des Chips kommt in rtoos mit der
# Chip-Bibliothek, und Bootloader und Firmware brauchen verschiedene.
cmake_minimum_required(VERSION 3.28)

get_filename_component(_repos "${CMAKE_SOURCE_DIR}/../../../.." ABSOLUTE)
set(RTOOS_DIR "${_repos}/rtoos" CACHE PATH "Checkout von klaus-liebler/rtoos")
if(NOT EXISTS "${RTOOS_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "${RTOOS_DIR}/CMakeLists.txt fehlt: rtoos neben sensact auschecken")
endif()

project(sensact_bootloader_f103 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

get_filename_component(F103_DIR "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
include(${F103_DIR}/cmake/f103_linker_script.cmake)
sensact_f103_linker_script("${CMAKE_BINARY_DIR}/sensact_f103_boot.ld" BOOTLOADER)
set(RTOS_STM32F103_LINKER_SCRIPT "${CMAKE_BINARY_DIR}/sensact_f103_boot.ld")

set(RTOS_SSH OFF)
add_subdirectory(${RTOOS_DIR} rtoos EXCLUDE_FROM_ALL)

rtos_add_firmware(sensact_f103_boot
    SOURCES main.cpp
    LIBRARIES rtos_stm32f103)
target_include_directories(sensact_f103_boot PRIVATE ${CMAKE_SOURCE_DIR} ${F103_DIR}/boot ${F103_DIR}/src)
target_compile_options(sensact_f103_boot PRIVATE -Wall -Wextra)
