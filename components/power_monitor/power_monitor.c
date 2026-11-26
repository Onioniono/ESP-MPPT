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

// Power Monitor Task
static void power_monitor_task(void *arg) {
    ina219_data_t data;
    while (1) {
        esp_err_t err
        = ina219_read(&data);
        while (1) {
            esp_err_t err = ina219_read(&data);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "INA219 readings - Voltage: %.2f V, Current: %.2f A, Power: %.2f W",
                data.voltage_v, data.current_a, data.power_w);
            } else {
                ESP_LOGE(TAG, "INA219 read failed: %s", esp_err_to_name(err));
            }
            vTaskDelay(pdMS_TO_TICKS(POWER_MONITOR_TASK_PERIOD_MS));
        }
    }
}

// Power Monitor Start
esp_err_t power_monitor_start(void) {
    BaseType_t result = xTaskCreate(
        power_monitor_task, "power_monitor_task", POWER_MONITOR_STACK_SIZE, NULL, POWER_MONITOR_TASK_PRIORITY, NULL);
    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to start power monitor task");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Power Montior Initialized");
    return ESP_OK;
}