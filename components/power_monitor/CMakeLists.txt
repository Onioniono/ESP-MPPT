idf_component_register(
    SRCS "power_optimize.c" "power_monitor.c"
    INCLUDE_DIRS "include"
    REQUIRES ina219
)