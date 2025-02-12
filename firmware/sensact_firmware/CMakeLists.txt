cmake_minimum_required(VERSION 3.5)
set(PROJECT_VER "0.0.0.1")

set(GENERATED_DIR "C:/repos/generated/")
set(USERPROFILE $ENV{USERPROFILE})

set(EXTRA_COMPONENT_DIRS "C:/repos/espidf-components" "C:/repos/sensact/espidf-components" "C:/repos/espidf-components-lcd" "C:/repos/espidf-component-webmanager")
set(EXCLUDE_COMPONENTS "C:/repos/espidf-components/esp32-owb")

if(DEFINED $ENV{DE_KLAUS_LIEBLER_BOARD_NAME})
    set(BOARD_NAME $ENV{DE_KLAUS_LIEBLER_BOARD_NAME})
else()
    set(BOARD_NAME "UNKNOWN")
endif()

if(DEFINED $ENV{DE_KLAUS_LIEBLER_BOARD_VERSION})
    set(BOARD_VERSION $ENV{DE_KLAUS_LIEBLER_BOARD_VERSION})
else()
    set(BOARD_VERSION 0)
endif()

if(DEFINED $ENV{DE_KLAUS_LIEBLER_BOARD_ROLES})
    set(BOARD_ROLES $ENV{DE_KLAUS_LIEBLER_BOARD_ROLES})
else()
    set(BOARD_ROLES "UNKNOWN")
endif()

if(DEFINED $ENV{DE_KLAUS_LIEBLER_BOARD_ROLES})
    set(NODE_ID $ENV{DE_KLAUS_LIEBLER_BOARD_ROLES})
else()
    set(NODE_ID "SNSCT_NODE_SIDEDOOR")
endif()




include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sensact_firmware)