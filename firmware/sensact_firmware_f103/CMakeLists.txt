# Sensact-Station auf dem STM32F103 (sensactUp Rev 0.2: SNSCT_L1_LVNG_UP, SNSCT_L1_KTCH_UP) mit rtoos.
#
#   dotnet run --project ../sensact_firmware/builder -- GenerateStm32F103Files --model Sattlerstrasse16 --nodeId SNSCT_L1_LVNG_UP
#   cmake --preset lvng && cmake --build --preset lvng        -> build/lvng/sensact_f103.elf/.bin
#
# Die Apps sind die der ESP32-Basis (espidf-components/sensact-applicationmodel); die Station
# bekommt ihre Konfiguration (BestBinaryBuffers, vom Builder aus dem XML-Hausmodell) einkompiliert.
# Nachbar-Repositories (wie bei den Host-Tests der Apps): rtoos und espidf-components neben sensact.
cmake_minimum_required(VERSION 3.28)

set(SENSACT_NODE "" CACHE STRING "Station (Hausmodell), z. B. SNSCT_L1_LVNG_UP - setzt das Preset")
get_filename_component(_repos "${CMAKE_SOURCE_DIR}/../../.." ABSOLUTE)
set(RTOOS_DIR "${_repos}/rtoos" CACHE PATH "Checkout von klaus-liebler/rtoos")
set(ESPIDF_COMPONENTS_DIR "${_repos}/espidf-components" CACHE PATH "Checkout von klaus-liebler/espidf-components")
set(SENSACT_GENERATED_DIR "${CMAKE_SOURCE_DIR}/../sensact_firmware/generated" CACHE PATH
    "Vom Builder erzeugte modellunabhaengige Dateien (common/*.inc, wsprotocol_cpp/ws_protocol.hh)")

project(sensact_firmware_f103 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

get_filename_component(SENSACT_REPO_DIR "${CMAKE_SOURCE_DIR}/../.." ABSOLUTE)
set(APPMODEL_DIR "${SENSACT_REPO_DIR}/espidf-components/sensact-applicationmodel")
set(STATION_CONFIG "${CMAKE_SOURCE_DIR}/generated/${SENSACT_NODE}/station_config.bin")

if(SENSACT_NODE STREQUAL "")
    message(FATAL_ERROR "SENSACT_NODE fehlt: mit einem Preset konfigurieren (cmake --list-presets)")
endif()
foreach(_f "${RTOOS_DIR}/CMakeLists.txt" "${ESPIDF_COMPONENTS_DIR}/common/include/common.hh"
           "${SENSACT_GENERATED_DIR}/sensact_model/common/parseCommand.inc"
           "${SENSACT_GENERATED_DIR}/wsprotocol_cpp/ws_protocol.hh" "${STATION_CONFIG}")
    if(NOT EXISTS "${_f}")
        message(FATAL_ERROR "${_f} fehlt. rtoos/espidf-components neben sensact auschecken bzw. zuerst den Builder "
                            "laufen lassen: GenerateStm32F103Files --model <Haus> --nodeId ${SENSACT_NODE}")
    endif()
endforeach()

set(RTOS_SSH OFF)
add_subdirectory(${RTOOS_DIR} rtoos EXCLUDE_FROM_ALL)

rtos_add_firmware(sensact_f103
    SOURCES
        src/main.cpp
        src/app_host.cpp
        src/heap.cpp
        src/log.cpp
        src/board/sensactup02.cpp
        # die Apps der ESP32-Basis (ohne Sound, Milight, Fingerprint, Jalousie-Timer: SENSACT_WITHOUT_...)
        ${APPMODEL_DIR}/cApplications.cc
        ${APPMODEL_DIR}/station_config.cc
        ${APPMODEL_DIR}/apps/blind.cc
        ${APPMODEL_DIR}/apps/lightbarrier.cc
        ${APPMODEL_DIR}/apps/node.cc
        ${APPMODEL_DIR}/apps/onoff.cc
        ${APPMODEL_DIR}/apps/rgbw.cc
        ${APPMODEL_DIR}/apps/rgbindicator.cc
        ${APPMODEL_DIR}/apps/singlepwm.cc
        ${ESPIDF_COMPONENTS_DIR}/common/common.cc
    LIBRARIES rtos_stm32f103)

target_include_directories(sensact_f103 PRIVATE
    # Ersatz-Header ZUERST: sie verdecken die gleichnamigen ESP-IDF-Header (esp_log.h, common-esp32.hh)
    src/stubs
    src
    ${APPMODEL_DIR}
    ${APPMODEL_DIR}/apps
    ${SENSACT_GENERATED_DIR}/sensact_model
    ${SENSACT_GENERATED_DIR}
    ${SENSACT_REPO_DIR}/espidf-components/sensactcommon
    ${SENSACT_REPO_DIR}/espidf-components/sensact-hal
    ${SENSACT_REPO_DIR}/espidf-components/sensact-middleware
    ${ESPIDF_COMPONENTS_DIR}/common/include
    ${ESPIDF_COMPONENTS_DIR}/errorcodes/include
    ${ESPIDF_COMPONENTS_DIR}/led)
target_compile_definitions(sensact_f103 PRIVATE
    SENSACT_WITHOUT_SOUND SENSACT_WITHOUT_FINGERPRINT SENSACT_WITHOUT_MILIGHT SENSACT_WITHOUT_BLINDSTIMER
    SENSACT_LOG_LEVEL=3
    SENSACT_HEAP_BYTES=8192)
target_compile_options(sensact_f103 PRIVATE -Wall -Wextra -Wno-unused-parameter "--embed-dir=${CMAKE_SOURCE_DIR}/generated/${SENSACT_NODE}")
set_source_files_properties(src/main.cpp PROPERTIES OBJECT_DEPENDS "${STATION_CONFIG}")
