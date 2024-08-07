# ESP32S3 GC032A Camera Driver

[![Build examples](https://github.com/espressif/esp32-camera/actions/workflows/build.yml/badge.svg)](https://github.com/espressif/esp32-camera/actions/workflows/build.yml) [![Component Registry](https://components.espressif.com/components/espressif/esp32-camera/badge.svg)](https://components.espressif.com/components/espressif/esp32-camera)

### Supported Soc

- ESP32-S3

### Supported Sensor

| model  | max resolution | color type | output format    |
| ------ | -------------- | ---------- | ---------------- |
| GC032A | 640 x 480      | color      | YCbCr422 /RGB565 |

### Factory Example Functions

- Set gc032a camera to 640x480 RGB565 mode
- Convert RGB565 to JPEG
- Send image data from UART0

### How to configure factory example?

- Select Octal Mode PSRAM in menuconfig
  ![image](data\psram.png)
- Select GC023A sensor and i2c0 bus in menuconfig
  ![image](data\demo_mecuconfig.png)

### Hardware connection

#### Psv2-dev

![image](data\psv2_dev.png)

![image](data\hardware_psv2_dev.png)

- UART0 pin definition

| GND | TX | RX |
| --- | -- | -- |
| GND | 43 | 44 |


- GC032A pin definition

| GC032A    | XCLK | SIOD     | SIOC      | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 | VSYNC | HREF |
| --------- | ---- | -------- | --------- | -- | -- | -- | -- | -- | -- | -- | -- | ----- | ---- |
| ESP32S3R8 | 6    | 2 [I2C0] | 0  [I2C0] | 46 | 42 | 41 | 45 | 5  | 8  | 7  | 39 | 3     | 4    |


- I2C-SLAVE pin definition

| VCC | GND | SLAVE_SDA | SLAVE_SCL |
| --- | --- | --------- | --------- |
| 3.3 | GND | 17 [I2C1] | 18 [I2C1] |

#### Psv2-1.0

![image](data\hardware_psv2_1.0.png)
![image](data\psv2_1.0.png)

- UART0 pin definition

| GND | TX | RX |
| --- | -- | -- |
| GND | 43 | 44 |


- GC032A pin definition

| GC032A    | XCLK | SIOD     | SIOC      | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 | VSYNC | HREF | RST |
| --------- | ---- | -------- | --------- | -- | -- | -- | -- | -- | -- | -- | -- | ----- | ---- | --- |
| ESP32S3R8 | 41   | 1 [I2C0] | 2  [I2C0] | 5  | 6  | 4  | 46 | 45 | 42 | 39 | 40 | 4     | 3    | 8   |


- I2C-SLAVE pin definition

| VCC | GND | SLAVE_SDA | SLAVE_SCL |
| --- | --- | --------- | --------- |
| 3.3 | GND | 17 [I2C1] | 18 [I2C1] |

### Sample test

- Download host app
  https://github.com/ArduCAM/Arducam_mini/releases/download/v1.0.1/ArduCAM_Host_V2.0_Windows.rar
- Test （select right serial port baudrate: 921600）
  ![image](data\demo.png)

### Write I2C-SLAVE  (7bits address: 0x0C)

```bash
* ___________________________________________________________________
* | start | slave_addr + wr_bit + ack | write n bytes + ack  | stop |
* --------|---------------------------|----------------------|------|
*
static esp_err_t __attribute__((unused)) i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
```

### Read I2C-SLAVE

```bash
 *_______________________________________________________________________________________
 * | start | slave_addr + rd_bit +ack | read n-1 bytes + ack | read 1 byte + nack | stop |
 * --------|--------------------------|----------------------|--------------------|------|
 *
 static esp_err_t __attribute__((unused)) i2c_master_read_slave(i2c_port_t i2c_num, uint8_t *data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
```
