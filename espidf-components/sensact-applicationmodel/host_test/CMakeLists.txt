# Host-Unit-Tests fuer die Sensact-Apps (sensact-applicationmodel/apps).
#
# Eigenstaendiges CMake-Projekt, OHNE ESP-IDF: die Apps haengen nur am Interface iSensactContext,
# deshalb lassen sie sich mit einem normalen Desktop-Compiler bauen und gegen einen Fake-Kontext
# testen. Bedienung und Hintergrund s. README.md in diesem Ordner.
cmake_minimum_required(VERSION 3.24)
project(sensact_apps_host_test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(APPMODEL_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(SENSACT_REPO_DIR "${APPMODEL_DIR}/../.." ABSOLUTE)

# Geschwister-Repo klaus-liebler/espidf-components (common.hh, errorcodes.hh, crgb.hh, ...).
# Gleiche Annahme wie EXTRA_COMPONENT_DIRS in firmware/sensact_firmware/CMakeLists.txt.
set(ESPIDF_COMPONENTS_DIR "${SENSACT_REPO_DIR}/../espidf-components" CACHE PATH
    "Checkout von klaus-liebler/espidf-components")
set(SENSACT_BUILDER_DIR "${SENSACT_REPO_DIR}/firmware/sensact_firmware/builder" CACHE PATH
    "C#-Builder von sensact_firmware (erzeugt die Modell-.inc-Dateien)")
# Leer lassen = Modell-Dateien beim Konfigurieren per Builder frisch erzeugen (empfohlen).
# Alternativ ein Verzeichnis mit bereits erzeugten Dateien angeben (Struktur wie die Ausgabe
# von "GenerateModelFilesForHostTests"), z.B. wenn kein .NET-SDK vorhanden ist.
set(SENSACT_MODEL_DIR "" CACHE PATH "Vorab erzeugte Modell-Dateien (optional)")

foreach(_dir IN ITEMS "${ESPIDF_COMPONENTS_DIR}/common/include" "${ESPIDF_COMPONENTS_DIR}/errorcodes/include" "${ESPIDF_COMPONENTS_DIR}/led")
    if(NOT EXISTS "${_dir}")
        message(FATAL_ERROR "Verzeichnis nicht gefunden: ${_dir} -- ESPIDF_COMPONENTS_DIR pruefen.")
    endif()
endforeach()

# ---------------------------------------------------------------------------------------------
# Modell-Dateien (.inc) aus dem Testmodel des Builders erzeugen. Bewusst bei JEDEM Konfigurieren
# frisch, damit die Tests nie gegen veraltete Generator-Ausgaben laufen (genau das war beim
# Anlegen dieser Tests mit firmware/sensact_firmware/generated/ der Fall). CONFIGURE_DEPENDS
# sorgt dafuer, dass CMake sich neu konfiguriert, sobald sich Generator oder Testmodel aendern.
# ---------------------------------------------------------------------------------------------
if(SENSACT_MODEL_DIR STREQUAL "")
    set(_model_out "${CMAKE_BINARY_DIR}/sensact_model")
    find_program(DOTNET_EXECUTABLE dotnet REQUIRED)
    get_filename_component(_fw_dir "${SENSACT_BUILDER_DIR}/.." ABSOLUTE)
    file(GLOB_RECURSE _builder_sources CONFIGURE_DEPENDS
        "${_fw_dir}/modellib/ModelGeneration/*.cs"
        "${_fw_dir}/model/*.xml"
        "${_fw_dir}/model/schema/*.xsd"
        "${_fw_dir}/best_binary_buffers_schema/*.cs"
        "${SENSACT_BUILDER_DIR}/*.cs")
    # Bei jeder inhaltlichen Aenderung dieser Dateien neu konfigurieren (und damit neu erzeugen) --
    # CONFIGURE_DEPENDS allein reagiert nur auf hinzugekommene/geloeschte Dateien.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${_builder_sources})
    message(STATUS "Erzeuge Modell-Dateien fuer Host-Tests -> ${_model_out}")
    execute_process(
        COMMAND "${DOTNET_EXECUTABLE}" run --project "${SENSACT_BUILDER_DIR}" -- GenerateModelFilesForHostTests --out "${_model_out}"
        RESULT_VARIABLE _gen_result
        OUTPUT_VARIABLE _gen_output
        ERROR_VARIABLE _gen_output)
    if(NOT _gen_result EQUAL 0)
        message(FATAL_ERROR "GenerateModelFilesForHostTests fehlgeschlagen:\n${_gen_output}")
    endif()
else()
    set(_model_out "${SENSACT_MODEL_DIR}")
endif()
if(NOT EXISTS "${_model_out}/common/parseCommand.inc")
    message(FATAL_ERROR "Keine Modell-Dateien unter ${_model_out}")
endif()

# ---------------------------------------------------------------------------------------------
# GoogleTest
# ---------------------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------------------------------------------
# Die Apps selbst und die App-Fabrik -- dieselben Quellen wie in ../CMakeLists.txt.
# ---------------------------------------------------------------------------------------------
add_library(sensact_apps STATIC
    "${APPMODEL_DIR}/cApplications.cc"
    "${APPMODEL_DIR}/station_config.cc"
    "${APPMODEL_DIR}/apps/blind.cc"
    "${APPMODEL_DIR}/apps/blindstimer.cc"
    "${APPMODEL_DIR}/apps/lightbarrier.cc"
    "${APPMODEL_DIR}/apps/milightcontroller.cc"
    "${APPMODEL_DIR}/apps/node.cc"
    "${APPMODEL_DIR}/apps/onoff.cc"
    "${APPMODEL_DIR}/apps/rgbw.cc"
    "${APPMODEL_DIR}/apps/rgbindicator.cc"
    "${APPMODEL_DIR}/apps/singlepwm.cc"
    "${APPMODEL_DIR}/apps/sound.cc"
    "${ESPIDF_COMPONENTS_DIR}/common/common.cc"
    support/host_log.cc)
target_include_directories(sensact_apps PUBLIC
    # Stubs ZUERST, damit sie gleichnamige ESP-IDF-Header (esp_log.h, common-esp32.hh) verdecken
    "${CMAKE_CURRENT_LIST_DIR}/stubs"
    "${APPMODEL_DIR}"
    "${APPMODEL_DIR}/apps"
    "${_model_out}"
    "${SENSACT_REPO_DIR}/espidf-components/sensactcommon"
    "${SENSACT_REPO_DIR}/espidf-components/sensact-hal"
    "${ESPIDF_COMPONENTS_DIR}/common/include"
    "${ESPIDF_COMPONENTS_DIR}/errorcodes/include"
    "${ESPIDF_COMPONENTS_DIR}/led")
# _POSIX_THREAD_SAFE_FUNCTIONS: gmtime_r/localtime_r unter MinGW
# SENSACT_WITH_MODEL_APPLICATION_IDS: die Tests sprechen Apps des Testmodells mit Namen an
# (eApplicationID::PUSHB_...); die Firmware kennt nur die Sonderwerte (s. applicationmodel_enums_and_structs.hh)
target_compile_definitions(sensact_apps PUBLIC _POSIX_THREAD_SAFE_FUNCTIONS SENSACT_HOST_TEST SENSACT_WITH_MODEL_APPLICATION_IDS
    SENSACT_HOST_TEST_MODEL_DIR="${_model_out}")
if(MSVC)
    target_compile_options(sensact_apps PRIVATE /W3)
else()
    target_compile_options(sensact_apps PRIVATE -Wall -Wno-unused-variable -Wno-unused-but-set-variable)
endif()

enable_testing()
file(GLOB _test_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/tests/*.cc")
add_executable(sensact_apps_tests support/FakeSensactContext.cc ${_test_sources})
target_include_directories(sensact_apps_tests PRIVATE "${CMAKE_CURRENT_LIST_DIR}/support")
target_link_libraries(sensact_apps_tests PRIVATE sensact_apps GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(sensact_apps_tests DISCOVERY_MODE PRE_TEST)
