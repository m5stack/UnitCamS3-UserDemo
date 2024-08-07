/**
 * @file hal_watch_dog.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-06-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_unitcam_s3_5mp.h"
#include "../hal_config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <thread>
#include <mutex>
#include <mooncake.h>

static uint8_t _dog = 0;
static std::mutex _feed_mutex;
static void _daemon_watch_dog(void* param)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        _feed_mutex.lock();
        _dog++;
        if (_dog > 6)
        {
            spdlog::error(":(");
            spdlog::error("watch dog timeout");
            spdlog::error("rebooting..");
            vTaskDelay(pdMS_TO_TICKS(100));

            esp_restart();
            // assert(false);
        }
        _feed_mutex.unlock();
    }
    vTaskDelete(NULL);
}

void HAL_UnitCamS3_5MP::_watch_dog_init()
{
    spdlog::info("watch dog init");
    // xTaskCreate(_daemon_watch_dog, "wd", 2000, NULL, configMAX_PRIORITIES - 1, NULL);

    // /* -------------------------------------------------------------------------- */
    // /*                                    Test                                    */
    // /* -------------------------------------------------------------------------- */
    // int b = 2;
    // while (1)
    // {
    //     // delay(5);
    //     int a = b * b;
    //     feedTheDog();
    // }
}

void HAL_UnitCamS3_5MP::feedTheDog()
{
    // _feed_mutex.lock();
    // _dog = 0;
    // _feed_mutex.unlock();

    // spdlog::info("??");
    vTaskDelay(1);
}
