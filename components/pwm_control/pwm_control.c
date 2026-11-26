#include "pwm_control.h"

#include <math.h>
#include <stdint.h>

#include "driver/ledc.h"
#include "esp_log.h"

#include "hardware_config.h"

#define MPPT_PWM_FREQ_HZ 100000
#define MPPT_PWM_RESOLUTION LEDC_TIMER_8_BIT

static const char *TAG = "PWM_CONTROL";

// PWM Timer & Channel Setup
esp_err_t pwm_control_init(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,      // Use low speed mode for control
        .timer_num = LEDC_TIMER_0,              // Use timer 0 for motor PWM
        .duty_resolution = MPPT_PWM_RESOLUTION, // 8-bit resolution duty cycle (0-255)
        .freq_hz = MPPT_PWM_FREQ_HZ,            // 100 kHz PWM frequency
        .clk_cfg = LEDC_AUTO_CLK                // Auto select source clock
    };
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,      // Use low speed mode for control
        .channel = LEDC_CHANNEL_0,              // Use channel 0 for motor PWM
        .timer_sel = LEDC_TIMER_0,              // Select timer 0 for motor PWM
        .intr_type = LEDC_INTR_DISABLE,         // Disable interrupts
        .gpio_num = PWM_PIN,                    // GPIO pin for PWM
        .duty = 0,                              // Start with 0% duty cycle (off)
        .hpoint = 0                             // Not used in this configuration
    };

    esp_err_t err = 
        ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "PWM timer initialization failed");
        return err;
    }
    err = 
        ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "PWM channel initialization failed");
        return err;
    }

    return ESP_OK;
}

// Set PWM & Direction
esp_err_t pwm_control_set_duty(float duty) {
    if (!isfinite(duty) || duty < 0.0f || duty > 1.0f) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t pwm = (uint32_t)lroundf(duty*255.0f);

    esp_err_t err =
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, pwm);
    if (err != ESP_OK) {
        return err;
    }

    err =
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    return err;
}