#include "power_optimize.h"
#include "power_monitor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_err.h"

#include "ina219.h"

/************************* 
Purpose: 
Optimize MPPT power by adjusting PWM output
*************************/

static const char *TAG = "POWER_OPTIMIZE";   // Tag for logging
static QueueHandle_t power_optimize_queue = NULL;
#define POWER_OPTIMIZE_TASK_STACK_SIZE 3072
#define POWER_OPTIMIZE_TASK_PRIORITY 4

#define MPPT_INITIAL_DUTY 0.50f
#define MPPT_DUTY_STEP 0.01f
#define MPPT_MIN_DUTY 0.20f
#define MPPT_MAX_DUTY 0.80f
#define MPPT_POWER_TOLERANCE 0.005f

// Clamp duty cycle
static float constrain_duty(float duty) {
    if (duty > MPPT_MAX_DUTY) {
        return MPPT_MAX_DUTY;
    }
    if (duty < MPPT_MIN_DUTY) {
        return MPPT_MIN_DUTY;
    }
    return duty;
}

// Power Monitor Task
static void power_optimize_task(void *pvParameters) {
    ina219_data_t data;

    float previous_power = 0.0f;
    float power_difference = 0.0f;

    float duty_cycle = MPPT_INITIAL_DUTY;
    float duty_step = MPPT_DUTY_STEP;

    int direction = 1; // +1 = increase ; -1 = decrease
    bool first_reading = true;

    while (1) {
        if (xQueueReceive(power_optimize_queue, &data, portMAX_DELAY) == pdTRUE) {
            if (first_reading) {
                previous_power = data.power_w;
                first_reading = false;
                continue;
            }

            power_difference = data.power_w - previous_power;
            if (power_difference < -MPPT_POWER_TOLERANCE) {
                direction = -direction;
            } else if (power_difference <= MPPT_POWER_TOLERANCE) {
                // Power approximately unchanged
            }
            
            float next_duty = duty_cycle + (direction*duty_step);
            next_duty = constrain_duty(next_duty);
            /*
            esp_err_t err = pwm_set_duty(next_duty);     // Currently unimplemented
            if (err != ESP_OK) {
                // Handle PWM failure
                continue;
            }
            */

            duty_cycle = next_duty; // Local duty-cycle state
            previous_power = data.power_w;
           
            ESP_LOGI(TAG, "V: %.3f V | I: %.3f A | P: %.3f W | " "Delta P: %+.3f W | Duty: %.1f%% | Direction: %d",
                    data.voltage_v, data.current_a, data.power_w, power_difference, duty_cycle * 100.0f, direction);
        }
    }
}

// Power Montior Start
esp_err_t power_optimize_start(void) {
    power_optimize_queue = xQueueCreate(1, sizeof(ina219_data_t));
    if (power_optimize_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create power optimize queue");
        return ESP_ERR_NO_MEM;
    }

    BaseType_t result
        = xTaskCreate(power_optimize_task, "power_optimize task", POWER_OPTIMIZE_TASK_STACK_SIZE, NULL, POWER_OPTIMIZE_TASK_PRIORITY, NULL);
    if (result != pdPASS) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

// Power Monitor Send Queue
esp_err_t power_optimize_send(const ina219_data_t *data) {
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (power_optimize_queue == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xQueueOverwrite(power_optimize_queue, data) != pdTRUE) {
        return ESP_FAIL;
    }
    return ESP_OK;
}