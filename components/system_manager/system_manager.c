#include "system_manager.h"
#include "system_config.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c_bus.h"
#include "ina219.h"
#include "power_monitor.h"

/************************* 
Purpose: 
Initialize, test, and manage system-wide services.
*************************/

static const char *TAG = "SYSTEM_MANAGER";      // Tag for logging
static QueueHandle_t system_queue = NULL;       // Private Queue Handle
#define SYSTEM_MANAGER_TASK_STACK_SIZE 3072
#define SYSTEM_MANAGER_TASK_PRIORITY 3

// System Manager Initialization
esp_err_t system_manager_init(void)
{
    // Initialize system manager resources here
    ESP_LOGI(TAG, "Initializing system manager...");
    
    // Initialize the I2C bus if enabled
    #if ENABLE_I2C_BUS
        ESP_ERROR_CHECK(i2c_bus_init());
    #endif
    
    // Initialize the INA219 sensor if enabled
    #if ENABLE_INA219
        ESP_ERROR_CHECK(ina219_init());
    #endif

    // Return ESP_OK to indicate successful initialization
    return ESP_OK;
}

// System Manager Start
esp_err_t system_manager_start(void)
{
    // Start system manager operations here
    ESP_LOGI(TAG, "Starting system manager...");

    // Start the environment service if BME280 is enabled
    #if ENABLE_POWER_MONITOR
        ESP_ERROR_CHECK(power_monitor_start());
    #endif

    // Return ESP_OK to indicate successful start
    return ESP_OK;
}

// System manager Test
esp_err_t system_manager_test(void) {
    // Start system manager operations here
    ESP_LOGI(TAG, "Starting system one-shot test(s)...");
    
    // INA219 sensor test
    #if TEST_INA219
        ina219_data_t data;
        while (1) {
            esp_err_t err = ina219_read(&data);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "INA219 readings - Voltage: %.2f V, Current: %.2f A, Power: %.2f W",
                data.voltage_v, data.current_a, data.power_w);
            } else {
                ESP_LOGE(TAG, "INA219 read failed: %s", esp_err_to_name(err));
            }
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    #endif

    // POWER_MONITOR test
    #if TEST_POWER_MONITOR
        // placeholder
    #endif

   return ESP_OK; // Placeholder return value for testing purposes
}