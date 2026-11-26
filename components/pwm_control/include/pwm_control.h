#pragma once

#include "esp_err.h"

esp_err_t pwm_control_init(void);
esp_err_t pwm_control_set_duty(float duty);