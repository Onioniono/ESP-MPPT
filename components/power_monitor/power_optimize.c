#include "power_monitor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "ina219.h"
#include "power_monitor.h"

/************************* 
Purpose: 
Optimize MPPT power by adjusting PWM output
*************************/

static const char *TAG = "POWER_OPTIMIZE";   // Tag for logging
static QueueHandle_t power_optimize_queue = NULL;
#define POWER_OPTIMIZE_TASK_STACK_SIZE 3072
#define POWER_OPTIMIZE_TASK PRIORITY 4

