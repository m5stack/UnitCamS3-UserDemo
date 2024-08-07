/**
 * @file hal_config.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-06-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

/**
 * @brief Pin configs
 *
 */

// I2C
#define HAL_PIN_I2C_INTER_SDA 45
#define HAL_PIN_I2C_INTER_SCL 0
#define HAL_PIN_I2C_EXTER_SDA 20
#define HAL_PIN_I2C_EXTER_SCL 19

// Button
#define HAL_PIN_BUTTON_A 0

// Led
#define HAL_PIN_LED 14

// MIC
#define HAL_PIN_MIC_CLK 47
#define HAL_PIN_MIC_DATA 48

// SD
#define HAL_PIN_SD_CS 9
#define HAL_PIN_SD_MOSI 38
#define HAL_PIN_SD_CLK 39
#define HAL_PIN_SD_MISO 40

// Camera
#define XCLK_FREQ_HZ 20000000

#define CAMERA_MODULE_NAME "M5Stack-Unit-CamS3"
#define CAMERA_PIN_PWDN -1
#define CAMERA_PIN_RESET 21

#define CAMERA_PIN_VSYNC 42
#define CAMERA_PIN_HREF 18
#define CAMERA_PIN_PCLK 12
#define CAMERA_PIN_XCLK 11

#define CAMERA_PIN_SIOD 17
#define CAMERA_PIN_SIOC 41

#define CAMERA_PIN_D0 6
#define CAMERA_PIN_D1 15
#define CAMERA_PIN_D2 16
#define CAMERA_PIN_D3 7
#define CAMERA_PIN_D4 5
#define CAMERA_PIN_D5 10
#define CAMERA_PIN_D6 4
#define CAMERA_PIN_D7 13
