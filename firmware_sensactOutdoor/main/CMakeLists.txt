# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(SRCS "aBlind.cc" "connect.c" "http_handlers.cc" "main.cc"
    EMBED_FILES ../webui_dist_compressed/index.html.gz
)
