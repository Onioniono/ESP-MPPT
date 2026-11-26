#include "ina219.h"

#include <stdint.h>

#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#include "i2c_bus.h"

/************************* 
Purpose: 
Initialize the INA219 currense sense.
*************************/

#define INA219_I2C_ADDRESS 0x40

#define INA219_REG_CONFIG 0x00
#define INA219_REG_SHUNT_VOLT 0x01
#define INA219_REG_BUS_VOLT 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURR 0x04
#define INA219_REG_CAL 0x05

#define INA219_CONFIG 0x3E67

#define INA219_I2C_FREQ_HZ 100000
#define INA219_I2C_TIMEOUT_MS 100

#define INA219_CALIBRATION_REG 0x05
#define INA219_CALIBRATION_VALUE 4096

static const char *TAG = "ina219";
static i2c_master_dev_handle_t ina219_handle = NULL;

// INA219 Write
static esp_err_t ina219_write_register(uint8_t reg, uint16_t value) {
    uint8_t buffer[3];

    buffer[0] = reg;
    buffer[1] = (uint8_t)(value >> 8);
    buffer[2] = (uint8_t)(value & 0xFF);

    return i2c_master_transmit(ina219_handle, buffer, sizeof(buffer), INA219_I2C_TIMEOUT_MS);
}

// INA219 Read
static esp_err_t ina219_read_registers(uint8_t start_reg, uint8_t *buffer, size_t length) {
    return i2c_master_transmit_receive(ina219_handle, &start_reg, sizeof(start_reg), buffer, length, INA219_I2C_TIMEOUT_MS);
}

// INA219 Initialization
/* Description:
- Obtain I2C Bus Handle
- Configure INA219 device with I2C address and settings
- Add INA219 device to I2C bus
- Configure INA219 measurement settings
*/
esp_err_t ina219_init(void) {
    // Get I2C Bus Handle
    i2c_master_bus_handle_t bus_handle = i2c_bus_get_handle();
    // FAILSAFE
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Verify INA219 responds over I2C
    esp_err_t err =
    i2c_master_probe(bus_handle, INA219_I2C_ADDRESS, INA219_I2C_TIMEOUT_MS);
    // FAILSAFE
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "INA219 not detected: %s", esp_err_to_name(err));
        return err;
    }

    // Configure INA219 Device
    i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = INA219_I2C_ADDRESS,
        .scl_speed_hz = INA219_I2C_FREQ_HZ,
    };

    // Add INA219 device to I2C Bus
    err =
    i2c_master_bus_add_device(bus_handle, &device_config, &ina219_handle);
    // FAILSAFE
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add INA219 to I2C bus: %s", esp_err_to_name(err));
        return err;
    }

    // Configure measurement settings
    /* Register Value: 0x3E67
    BRNG [13]: Bus Voltage measurement range — 32V (1'b1)
    PGA [12:11]: Shunt Voltage measurement range — +320mV (1'b11)
    BADC [10:7]: Bus Volttage ADC resolution/averaging — 12-bit, 16 samples (1'b1100)
    SADC [6:3]: Shunt Voltage ADC resolution/averaging — 12-bit, 16 samples (1'b1100)
    MODE [2:0]: Operating mode — Shunt & Bus, continuous (1'b111)
    */
    err =
    ina219_write_register(INA219_REG_CONFIG, INA219_CONFIG);
    // FAILSAFE
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure initial settings");
        return err;
    }

    return ESP_OK;
}

// INA219 Read Sensor Data
/* Notes:
Vsource = Vbus + Vshunt
Isource = Vshunt / 0.1
Psource = Vsource * Isource
- Shunt Resistance = 0.1 ohms
*/
esp_err_t ina219_read(ina219_data_t *data) {
    // FAILSFE
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Read raw data (shunt & bus voltages)
    uint8_t raw_shunt[2];
    uint8_t raw_bus[2];
     // raw_shunt
    esp_err_t err =
    ina219_read_registers(0x01, raw_shunt, sizeof(raw_shunt));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read shunt voltage: %s", esp_err_to_name(err));
        return err;
    }
     // raw_bus
    err =
    ina219_read_registers(0x02, raw_bus, sizeof(raw_bus));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read bus voltage: %s", esp_err_to_name(err));
        return err;
    }
    
    // Shift, combine, & convert raw data bytes
     // shift_shunt
    int16_t shift_shunt = (int16_t)(((uint16_t)raw_shunt[0] << 8) | raw_shunt[1]);
    data->current_a = (shift_shunt * 0.00001f) / 0.1f;
     // shift_bus
    uint16_t shift_bus = (((uint16_t)raw_bus[0] << 8 | raw_bus[1]) >> 3);
        // ESP_LOGI(TAG, "Raw Bus Bytes: 0x%02X 0x%02X", raw_bus[0], raw_bus[1]);
    data->voltage_v = shift_bus * 0.004f;
     // power
    data->power_w = data->voltage_v * data->current_a;

    return ESP_OK;
}