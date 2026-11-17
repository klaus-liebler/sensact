# Apps der Basis-Firmware. Welche Apps eine Station hat, steht nicht mehr im Code, sondern in der
# Stationskonfiguration (station_config.cc liest sie, cApplications haelt das Register). Aus dem
# generierten Verzeichnis werden nur noch modellunabhaengige Dateien gebraucht (Kommandos, Events)
# und der BestBinaryBuffers-Decoder (wsprotocol_cpp/ws_protocol.hh).
idf_component_register(SRCS "cApplications.cc" "station_config.cc" "apps/blind.cc" "apps/blindstimer.cc"  "apps/lightbarrier.cc" "apps/milightcontroller.cc" "apps/node.cc" "apps/onoff.cc" "apps/rgbw.cc" "apps/singlepwm.cc" "apps/sound.cc"
                    INCLUDE_DIRS "." "${GENERATED_DIR}/sensact_model" "${GENERATED_DIR}"
                    REQUIRES "sensactcommon" "sensact-hal" "led" "common"
                    EMBED_FILES "./dingdong.mp3" "./siren.mp3" "./positive.mp3" "./negative.mp3")
