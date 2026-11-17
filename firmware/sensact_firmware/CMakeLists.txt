cmake_minimum_required(VERSION 3.30)
set(PROJECT_VER "0.0.0.1")

set(GENERATED_DIR "${CMAKE_CURRENT_LIST_DIR}/generated/")
set(USERPROFILE $ENV{USERPROFILE})

set(EXTRA_COMPONENT_DIRS "C:/repos/espidf-components" "C:/repos/sensact/espidf-components" "C:/repos/espidf-components-lcd" "C:/repos/espidf-component-webmanager")

file(READ "${GENERATED_DIR}/cmake/config.json" config_json)
# Neu konfigurieren, sobald der Builder eine andere config.json schreibt (anderes Board/andere Station):
# sonst blieben NODE_ID, HAL_DIR (Include-Pfad der Board-Initialisierung) usw. aus dem vorherigen
# Konfigurieren stehen, und die Firmware wuerde mit der Board-Init des zuletzt gebauten Boards gebaut.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${GENERATED_DIR}/cmake/config.json")
string(JSON SENSACT_NODE_NAME GET "${config_json}" SENSACT_NODE_NAME)
string(JSON SENSACT_MODEL_NAME GET "${config_json}" SENSACT_MODEL_NAME)
string(JSON SENSACT_MODEL_CREATION_DT GET "${config_json}" SENSACT_MODEL_CREATION_DT)
string(JSON SENSACT_MODEL_CREATION_DT_STRING GET "${config_json}" SENSACT_MODEL_CREATION_DT_STRING)
string(JSON NODE_ID GET "${config_json}" NODE_ID)
# Board-Initialisierung je Board-TYP (main/hal/<HAL_DIR>/board_init.hh), nicht je Station
string(JSON HAL_DIR GET "${config_json}" HAL_DIR)
string(JSON HOSTNAME GET "${config_json}" HOSTNAME)
string(JSON BOARD_NAME GET "${config_json}" BOARD_NAME)
string(JSON BOARD_VERSION GET "${config_json}" BOARD_VERSION)
string(JSON BOARD_MAC GET "${config_json}" BOARD_MAC)
string(JSON APP_NAME GET "${config_json}" APP_NAME)
string(JSON APP_VERSION GET "${config_json}" APP_VERSION)
string(JSON CREATION_DT GET "${config_json}" CREATION_DT)
string(JSON CREATION_DT_STR GET "${config_json}" CREATION_DT_STR)
string(JSON GIT_SHORT_HASH GET "${config_json}" GIT_SHORT_HASH)
string(JSON BANNER GET "${config_json}" BANNER)
string(JSON BOARD_DIRECTORY GET "${config_json}" BOARD_DIRECTORY)

message(STATUS "Build for BOARD:${BOARD_NAME}, SENSACT_NODE_NAME:${SENSACT_NODE_NAME}, MAC:${BOARD_MAC}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sensact_firmware)