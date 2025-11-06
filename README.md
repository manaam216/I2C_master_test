# I2C_master_test
Test I2C Code for Master side for ESP32 Devices 

This example demonstrates how to use the **modern I2C master driver API** in ESP-IDF (v5.4+), to communicate with an I²C slave device.  
It performs simple register **read** and **write** operations at 400 kHz (Fast Mode I²C).

---

## Features

- Uses **new `i2c_master_bus_*` APIs** introduced in ESP-IDF 5.x  
- Demonstrates **repeated-start read** using `i2c_master_transmit_receive()`  
- Writes and reads registers on an I²C slave (address `0x3A`)  
- Uses internal pull-ups for convenience  
- Logs all operations and errors with detailed feedback  

---

## Project Structure

```
i2c_master_modern/
├── main/
│   ├── CMakeLists.txt
│   └── main.c
├── CMakeLists.txt
└── README.md
```

---

## Hardware Setup

| Signal | ESP32 Pin | Description      |
|:--------|:-----------|:----------------|
| SDA     | GPIO19     | I²C Data Line   |
| SCL     | GPIO18     | I²C Clock Line  |
| GND     | Common     | Shared ground   |

> The slave device should be connected to 3.3V logic and support I²C Fast Mode (400 kHz).  
> Internal pull-ups are enabled by default, but for stable operation, use **external 4.7kΩ pull-ups** to 3.3V on SDA and SCL.

---

## Software Requirements

- **ESP-IDF v5.1 or later**
- **FreeRTOS** (included in ESP-IDF)
- Supported targets: **ESP32**, **ESP32-S2**, **ESP32-S3**, **ESP32-C3**, etc.

---

## Build and Flash

```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Configure (optional)
idf.py menuconfig

# Build and flash to your board
idf.py -p /dev/ttyUSB0 flash

# Monitor logs
idf.py monitor
```

---

## Example Output

Expected UART log output:
```
I (0) i2c_master_modern: I2C master initialized using new driver API
I (3030) i2c_master_modern: Read OK: Reg 0x00 => 0x12 0x34 0x56 0x78
I (6040) i2c_master_modern: Write OK: Reg 0x02 <= 0x03
I (7050) i2c_master_modern: Write OK: Reg 0x03 <= 0x04
I (10080) i2c_master_modern: Read OK: Reg 0x01 => 0x01 0x00 0x00 0x00
```

If there’s an issue:
```
E (1040) i2c_master_modern: Write FAIL: ESP_ERR_TIMEOUT
E (4060) i2c_master_modern: Read FAIL: ESP_ERR_INVALID_STATE
```

---

## Code Overview

### Initialize I²C Master
```c
i2c_master_bus_config_t bus_cfg = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_NUM_0,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
i2c_new_master_bus(&bus_cfg, &i2c_bus);
```

### Write Register
```c
uint8_t buf[2] = { reg, data };
i2c_master_transmit(i2c_slave_dev, buf, sizeof(buf), -1);
```

### Read Register (Repeated Start)
```c
i2c_master_transmit_receive(i2c_slave_dev, &reg, 1, data, 4, -1);
```

---

## Troubleshooting

| Issue | Possible Cause | Fix |
|:------|:----------------|:----|
| `ESP_ERR_TIMEOUT` | No ACK from slave | Check address and wiring |
| `ESP_ERR_INVALID_ARG` | Wrong parameter or uninitialized handle | Ensure device/bus created correctly |
| `Read FAIL` or all 0xFF | Pull-ups too weak | Use 4.7kΩ pull-ups to 3.3V |

---

## References

- [ESP-IDF I²C Master Driver (Modern API)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)
- [ESP-IDF Examples – I2C](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2c)

---

