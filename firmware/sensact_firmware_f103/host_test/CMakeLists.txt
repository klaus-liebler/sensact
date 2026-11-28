# Host-Tests des Bootloader-Protokolls (boot/boot_protocol.hpp): Empfangsautomat, Image-Pruefung, CRC.
# Ohne rtoos und ohne Hardware, mit einem normalen PC-Compiler:
#
#   cmake -S host_test -B build/host_test -G Ninja && cmake --build build/host_test && ctest --test-dir build/host_test
cmake_minimum_required(VERSION 3.24)
project(sensact_boot_host_test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()
add_executable(boot_protocol_tests test_boot_protocol.cpp)
target_include_directories(boot_protocol_tests PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../boot)
target_compile_options(boot_protocol_tests PRIVATE -Wall -Wextra -Werror)
target_link_libraries(boot_protocol_tests PRIVATE GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(boot_protocol_tests DISCOVERY_MODE PRE_TEST)

# Sende-Werkzeug (tools/sensact_f103_flash.py) gegen denselben Automaten: boot_sim + Python-Test
add_executable(boot_sim boot_sim.cpp)
target_include_directories(boot_sim PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../boot)
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
    add_test(NAME sensact_f103_flash_tool
             COMMAND ${Python3_EXECUTABLE} -I ${CMAKE_CURRENT_LIST_DIR}/test_sensact_f103_flash.py $<TARGET_FILE:boot_sim>)
endif()
if(TEST sensact_f103_flash_tool)
    set_tests_properties(sensact_f103_flash_tool PROPERTIES SKIP_RETURN_CODE 77)
endif()
