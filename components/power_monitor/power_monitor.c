#include "power_monitor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "ina219.h"

#define POWER_MONITOR_TASK_PERIOD_MS 5000 // 5 seconds
#define POWER_MONITOR_STACK_SIZE 4096
#define POWER_MONITOR_TASK_PRIORITY 5

static const char *TAG = "POWER_MONITOR";   // Tag for logging