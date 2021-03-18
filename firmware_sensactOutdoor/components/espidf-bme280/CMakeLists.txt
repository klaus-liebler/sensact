idf_component_register(SRCS "bme280.cc" "bme280_lowlevel.c"
                       INCLUDE_DIRS "include"
                       REQUIRES i2c)

