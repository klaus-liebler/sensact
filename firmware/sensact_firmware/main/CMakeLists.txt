
idf_component_register(
    SRCS "journal_plugin.cc" "node_applicationhost.cc" "model_node.cc" "main.cc"
    INCLUDE_DIRS "." "hal/${NODE_ID}" "${GENERATED_DIR}"
    REQUIRES "sensact-applicationmodel" "sensact-middleware" "espidf-component-webmanager" "adc_buttons" "audio" "common" "ethernet" "errorcodes" "flatbuffers-25.1.24" "i2c" "i2c_sensor" "ledanimator" "pca9555" "pca9685" "ringtones" "tas580x" "lcd_spi_16bpp" "lcd_common" "fatfs" "esp_http_server" "driver"
    EMBED_FILES "${GENERATED_DIR}/web/index.compressed.br"
    EMBED_TXTFILES "${BOARD_DIRECTORY}/certificates/esp32.pem.crt" "${BOARD_DIRECTORY}/certificates/esp32.pem.key" "${USERPROFILE}/netcase/certificates/rootCA.pem.crt"
)

littlefs_create_partition_image(storage ../littlefs_filesystem FLASH_IN_PROJECT)