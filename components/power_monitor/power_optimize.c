#include "power_monitor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "ina219.h"
#include "power_monitor.h"

static const char *TAG = "POWER_OPTIMIZE";   // Tag for logging