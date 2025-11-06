#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>

#define I2C_MASTER_SDA_IO          19
#define I2C_MASTER_SCL_IO          18
#define I2C_MASTER_FREQ_HZ         400000
#define I2C_SLAVE_ADDR             0x3A

static const char *TAG = "i2c_master_modern";

static i2c_master_bus_handle_t i2c_bus = NULL;
static i2c_master_dev_handle_t i2c_slave_dev = NULL;

static esp_err_t i2c_master_init(void)
{
    // Create the I2C bus
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &i2c_bus));

    // Attach slave device to bus
    i2c_device_config_t dev_cfg = {
        .device_address = I2C_SLAVE_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &dev_cfg, &i2c_slave_dev));

    ESP_LOGI(TAG, "I2C master initialized using new driver API");
    return ESP_OK;
}

/**
 * @brief Write a single register
 */
static esp_err_t i2c_master_write_slave_reg(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = { reg, data };
    esp_err_t ret = i2c_master_transmit(i2c_slave_dev, buf, sizeof(buf), -1);
    if (ret == ESP_OK)
        ESP_LOGI(TAG, "Write OK: Reg 0x%02X <= 0x%02X", reg, data);
    else
        ESP_LOGE(TAG, "Write FAIL: %s", esp_err_to_name(ret));
    return ret;
}

/**
 * @brief Read a single register (write address + repeated-start read)
 */
static esp_err_t i2c_master_read_slave_reg(uint8_t reg, uint8_t *data)
{
    esp_err_t ret = i2c_master_transmit_receive(
        i2c_slave_dev, &reg, 1, data, 4, -1);
    if (ret == ESP_OK)
        ESP_LOGI(TAG, "Read OK: Reg 0x%02X => 0x%02X %d %d %d", reg, data[0], data[1], data[2], data[3]);
    else
        ESP_LOGE(TAG, "Read FAIL: %s", esp_err_to_name(ret));
    return ret;
}

void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    uint8_t val[4];

    while (1) {
         vTaskDelay(pdMS_TO_TICKS(3000));
        memset(val,0,4);
        i2c_master_read_slave_reg(0x00, val);   // DEVICE_ID
        vTaskDelay(pdMS_TO_TICKS(3000));

        i2c_master_write_slave_reg(0x02, 0x03);
        vTaskDelay(pdMS_TO_TICKS(1000));
        i2c_master_write_slave_reg(0x03, 0x04);
        vTaskDelay(pdMS_TO_TICKS(3000));

        memset(val,0,4);
        i2c_master_read_slave_reg(0x01, val); 
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
