if(NOT DEFINED NODE_ID)
    set(NODE_ID "MONITORING")
endif()

idf_component_register(
    SRCS "node_applicationhost.cc" "model_node.cc" "main.cc" "ui/ui.c" "ui/ui_events.cpp" "ui/ui_helpers.c"
    INCLUDE_DIRS "." "hal/${NODE_ID}"
)
