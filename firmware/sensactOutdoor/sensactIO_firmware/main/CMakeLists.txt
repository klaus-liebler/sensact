idf_component_register(
    SRCS "http_handlers.cc" "main.cc" "manager.cc" "aBlind.cc" "aOnOff.cc" "aRgbwPwm.cc" "aSinglePwm.cc"
    INCLUDE_DIRS "."
    EMBED_FILES ../../sensactIO_webui/dist_compressed/index.html.gz
)
spiffs_create_partition_image(storage ../spiffs_filesystem FLASH_IN_PROJECT)
