#pragma once
#include "esp_err.h"
#include <stdbool.h>

esp_err_t system_manager_init(void);
esp_err_t system_manager_test(void);
esp_err_t system_manager_start(void);