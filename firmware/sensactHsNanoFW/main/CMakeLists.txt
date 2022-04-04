idf_component_register(
    SRCS "model_node.cc" "model_applications.cc" "node_applicationhost.cc" "main.cc" "apps/onoff.cc" "apps/blind.cc" "apps/singlepwm.cc"
    INCLUDE_DIRS "." "generated" "generated/SNSCT_L3_TECH_HS_1"
    EMBED_TXTFILES sciebo.cer
)
