#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

typedef struct {
    float voltage_v;
    float current_a;
    float power_w;
} ina219_data_t;

esp_err_t ina219_init(void);                    // Initialize the INA219 sensor and configure it for measurement
esp_err_t ina219_read(ina219_data_t *data);     // Read voltage & current values from INA219 and store in provided structure