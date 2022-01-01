idf_component_register(
    SRCS "can_id_utils.cpp" "nodemaster.cc" "busmaster.cc" "main.cpp"
    INCLUDE_DIRS ""
    EMBED_TXTFILES sciebo.cer
)
