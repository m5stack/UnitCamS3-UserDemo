/**
 * @file hal_mic.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-08-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_unitcam_s3_5mp.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <Arduino.h>
#include <LittleFS.h>
#include <utility/Mic_Class.hpp>

static m5::Mic_Class* _mic = nullptr;
m5::Mic_Class* __get_mic() { return _mic; }

void HAL_UnitCamS3_5MP::_mic_init()
{
    spdlog::info("mic init");

    assert(_mic == nullptr);

    _mic = new m5::Mic_Class;
    auto mic_config = _mic->config();
    mic_config.pin_data_in = HAL_PIN_MIC_DATA;
    mic_config.pin_ws = HAL_PIN_MIC_CLK;
    mic_config.task_priority = 5;
    mic_config.magnification = 16;
    // mic_config.noise_filter_level = 1;
    // mic_config.over_sampling = 4;
    // mic_config.over_sampling = 1;
    // mic_config.sample_rate = 48000;
    _mic->config(mic_config);
    _mic->begin();
    if (!_mic->begin())
    {
        popFatalError("mic init failed");
    }
    spdlog::info("ok");

    /* -------------------------------------------------------------------------- */
    /*                                    Test                                    */
    /* -------------------------------------------------------------------------- */
    // int16_t shit[100];
    // while (1)
    // {
    //     _mic->record(shit, 100);
    //     while (_mic->isRecording())
    //         {delay(1);}

    //     for (int i = 0; i < 100; i++)
    //         printf("m:%d\n", shit[i]);

    //     feedTheDog();
    // }
}
