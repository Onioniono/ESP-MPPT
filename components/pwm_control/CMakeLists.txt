idf_component_register(
    SRCS "pwm_control.c"
    INCLUDE_DIRS "include"
    REQUIRES hardware_config
    PRIV_REQUIRES esp_driver_ledc
)