idf_component_register(
    SRCS "system_manager.c"
    INCLUDE_DIRS "include"
    REQUIRES i2c_bus ina219 power_monitor pwm_control
)