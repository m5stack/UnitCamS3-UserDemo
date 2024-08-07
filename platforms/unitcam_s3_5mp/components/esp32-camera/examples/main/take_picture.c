/**
 * This example takes a picture every 5s and print its size on serial monitor.
 */

// =============================== SETUP ======================================

// 1. Board setup (Uncomment):
// #define BOARD_WROVER_KIT
// #define BOARD_ESP32CAM_AITHINKER

/**
 * 2. Kconfig setup
 *
 * If you have a Kconfig file, copy the content from
 *  https://github.com/espressif/esp32-camera/blob/master/Kconfig into it.
 * In case you haven't, copy and paste this Kconfig file inside the src directory.
 * This Kconfig file has definitions that allows more control over the camera and
 * how it will be initialized.
 */

/**
 * 3. Enable PSRAM on sdkconfig:
 *
 * CONFIG_ESP32_SPIRAM_SUPPORT=y
 *
 * More info on
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-esp32-spiram-support
 */

// ================================ CODE ======================================

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "img_converters.h"


// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "esp_camera.h"

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)


//ESP32S3-EYE
#define CAM_PIN_PWDN 38
#define CAM_PIN_RESET -1
#define CAM_PIN_VSYNC 6
#define CAM_PIN_HREF 7
#define CAM_PIN_PCLK 13
#define CAM_PIN_XCLK 15
#define CAM_PIN_SIOD 4
#define CAM_PIN_SIOC 5
#define CAM_PIN_D0 11
#define CAM_PIN_D1 9
#define CAM_PIN_D2 8
#define CAM_PIN_D3 10
#define CAM_PIN_D4 12
#define CAM_PIN_D5 18
#define CAM_PIN_D6 17
#define CAM_PIN_D7 16


static const char *TAG = "";

#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)


size_t _jpg_buf_len = 0;
uint8_t * _jpg_buf = NULL;
char start_stream = 0;
static QueueHandle_t uart0_queue;

#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    .xclk_freq_hz = 24000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
    
   //.frame_size = FRAMESIZE_96X96,
   //.frame_size = FRAMESIZE_128x128,
   //.frame_size = FRAMESIZE_320x320,
   .frame_size = FRAMESIZE_QVGA,
   //.frame_size = FRAMESIZE_VGA,
   //.frame_size = FRAMESIZE_HD,    
   //.frame_size = FRAMESIZE_UXGA,
   //.frame_size = FRAMESIZE_FHD,
   //.frame_size = FRAMESIZE_5MP,


    .brightness_t = brightness_0,      // default
    //.brightness_t = brightness_1,      // +1
    //brightness_t = brightness_2,      // -1
    //.brightness_t = brightness_3,      // +2
    //.brightness_t = brightness_4,      // -2
    //.brightness_t = brightness_5,      // +3
    //.brightness_t = brightness_7,      // +4
    //.brightness_t = brightness_8,      // -4

    .contrast_t = contrast_0,
    // .contrast_t = contrast_1,
    // .contrast_t = contrast_2,
    //.contrast_t = contrast_3,
    //.contrast_t = contrast_4,
    // .contrast_t = contrast_5,
    // .contrast_t = contrast_6,

    .saturation_t = saturation_0,
     //.saturation_t = saturation_1,
    // .saturation_t = saturation_2,
    // .saturation_t = saturation_3,
    // .saturation_t = saturation_4,
    // .saturation_t = saturation_5,
    // .saturation_t = saturation_6,

    .exposure_t = exposure_0,
    //.exposure_t = exposure_1,
    // .exposure_t = exposure_2,
    // .exposure_t = exposure_3,
    // .exposure_t = exposure_4,
    //.exposure_t = exposure_5,
    // .exposure_t = exposure_6,

    .awb_mode = Auto,
    //.awb_mode = sunny,
    // .awb_mode = office,
    //.awb_mode = cloudy,
    // .awb_mode = home,

    .special = normal,
     //.special = blueish,
    // .special = redish,
    // .special = BorW,
    // .special = sepia,
    // .special = negative,
    // .special = greenish,

    //.image_quality = quality_high,
    .image_quality = quality_default,
    // .image_quality = quality_low,

    .agc_mode = AGC_Auto,
     //.agc_mode = AGC_Manual,

    .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 2,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init_camera(void)
{
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}
#endif

void app_main(void)
{

    uart_config_t uart_config = {
        .baud_rate = 921600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    //Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);

    //Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);
    //Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
    //Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(EX_UART_NUM, 20);

#if ESP_CAMERA_SUPPORTED
    if(ESP_OK != init_camera()) {
        return;
    }
   start_stream = 1;
    while (1)
    {
    //  ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();
        if(pic->format != PIXFORMAT_JPEG){
            bool jpeg_converted = frame2jpg(pic, 70, &_jpg_buf, &_jpg_buf_len);
            if(jpeg_converted)
                uart_write_bytes(EX_UART_NUM, (const char*)_jpg_buf,  _jpg_buf_len); 
            if(_jpg_buf)
                free(_jpg_buf);
        }else{
            uart_write_bytes(EX_UART_NUM, (const char*)pic->buf,  pic->len); 
        }
       
        esp_camera_fb_return(pic);
        vTaskDelay(10 / portTICK_RATE_MS);
 
    }
#else
    ESP_LOGE(TAG, "Camera support is not available for this chip");
    return;
#endif
}
