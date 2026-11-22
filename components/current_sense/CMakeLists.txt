idf_component_register(
    SRCS "current_sense.c"
    INCLUDE_DIRS "include"
    REQUIRES i2c_bus esp_driver_i2c
)