idf_component_register(SRCS "model_applications.cc" "apps/blind.cc" "apps/node.cc" "apps/onoff.cc" "apps/rgbw.cc" "apps/singlepwm.cc" "apps/sound.cc"
                    INCLUDE_DIRS "."
                    REQUIRES "sensactcommon" "sensact-hal" "generated" "rgbled"
                    EMBED_FILES "./dingdong.mp3")
