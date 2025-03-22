idf_component_register(SRCS "model_applications.cc" "apps/blind.cc" "apps/blindstimer.cc"  "apps/milightcontroller.cc" "apps/node.cc" "apps/onoff.cc" "apps/rgbw.cc" "apps/singlepwm.cc" "apps/sound.cc"
                    INCLUDE_DIRS "." "${GENERATED_DIR}/sensact_model" "${GENERATED_DIR}/sensact_model/${NODE_ID}"
                    REQUIRES "sensactcommon" "sensact-hal" "rgbled"
                    EMBED_FILES "./dingdong.mp3")
