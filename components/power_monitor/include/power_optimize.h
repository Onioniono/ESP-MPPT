#pragma once

#include "esp_err.h"
#include"ina219.h"

esp_err_t power_optimize_start(void);
esp_err_t power_optimize_send(const ina219_data_t *data);