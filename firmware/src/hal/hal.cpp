/**
 * @file hal.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include <Arduino.h>
#include "hal.h"


using namespace HAL;


void hal::init()
{
    spdlog::set_pattern("[%M:%S] [%^%L%$] %v");
    spdlog::info("hal init");


    // Cam 
    _cam_init();


    // Mic 
    mic = new m5::Mic_Class;
    auto mic_config = mic->config();
    mic_config.pin_data_in = HAL_PIN_MIC_DATA;
    mic_config.pin_ws = HAL_PIN_MIC_CLK;
    mic_config.task_priority = 5;
    mic_config.magnification = 1;
    // mic_config.noise_filter_level = 1;
    // mic_config.over_sampling = 4;
    // mic_config.over_sampling = 1;
    // mic_config.sample_rate = 48000;
    mic->config(mic_config);
    mic->begin();


    // Led
    gpio_reset_pin(GPIO_NUM_14);
    pinMode(14, OUTPUT);
    setLed(false);

    // Config
    _config_init();
}


void hal::setLed(bool state)
{
    if (state)
        digitalWrite(14, 0);
    else 
        digitalWrite(14, 1);
}


hal* hal::_hal = nullptr;

hal* hal::GetHal()
{
    if (_hal == nullptr)
    {
        _hal = new hal;
    }
    return _hal;
}
