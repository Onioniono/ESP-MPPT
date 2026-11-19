#pragma once
#include "driver/gpio.h"

// ------------------------------
// Hardware Configuration
/*
Description:
This header file defines the hardware configuration
Notes:
- The GPIO pin assignments are defined using the ESP32 GPIO numbering scheme.
- The configuration is intended to be used across the project to ensure consistent hardware access.
*/
// ------------------------------

#define I2C_SDA_GPIO GPIO_NUM_21
#define I2C_SCL_GPIO GPIO_NUM_22