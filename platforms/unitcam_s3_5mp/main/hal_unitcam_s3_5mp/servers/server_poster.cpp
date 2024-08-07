/**
 * @file server_poster.cpp
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
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "apis/apis.h"
#include <assets/assets.h>

void HAL_UnitCamS3_5MP::startPosterServer()
{
    spdlog::info("start poster server");

    // If need to wait ap connection for 10s
    if (HAL::GetSystemConfig().waitApFirst == "yes")
    {
        spdlog::info("start wait ap");
        startApServer();

        uint32_t ap_wait_time_cout = millis();
        bool led_state = true;
        while (millis() - ap_wait_time_cout < 10000)
        {
            // If get client
            if (WiFi.softAPgetStationNum() != 0)
            {
                // Hold Ap mode
                spdlog::info("get client, hold ap mode");
                HAL::SetLedState(true);
                while (WiFi.softAPgetStationNum() != 0)
                    delay(2000);

                // If no config setting, reset counting
                ap_wait_time_cout = millis();
            }

            delay(500);
            led_state = !led_state;
            HAL::SetLedState(led_state);
        }

        spdlog::info("no client, go on poster mode");
        stopApServer();
    }

    // Try connect wifi
    spdlog::info("try conncet:\n{}\n{}", HAL::GetSystemConfig().wifiSsid, HAL::GetSystemConfig().wifiPass);
    WiFi.mode(WIFI_STA);
    WiFi.begin(HAL::GetSystemConfig().wifiSsid.c_str(), HAL::GetSystemConfig().wifiPass.c_str());

    // if (WiFi.waitForConnectResult() != WL_CONNECTED)
    // {
    //     spdlog::error("connect failed, return back to ap mode");
    //     config.start_poster = "no";
    //     HAL::hal::GetHal()->setConfig(config);

    //     // Reboot
    //     delay(300);
    //     esp_restart();
    //     delay(10000);
    // }

    {
        gpio_reset_pin(GPIO_NUM_14);
        pinMode(14, OUTPUT);
        ledcAttach(14, 12000, 8);
        uint32_t time_count = millis();
        bool is_connected = false;
        while (1)
        {
            for (int i = 0; i < 255; i++)
            {
                ledcWrite(14, i);
                delay(5);
            }

            for (int i = 255; i > 0; i--)
            {
                ledcWrite(14, i);
                delay(5);

                // Connected
                if (WiFi.status() == WL_CONNECTED)
                {
                    spdlog::info("connected");
                    is_connected = true;
                    break;
                }

                // Time out
                if (millis() - time_count > 30000UL)
                {
                    // spdlog::error("connect failed, return back to ap mode");
                    // config.start_poster = "no";
                    // HAL::hal::GetHal()->setConfig(config);

                    // Keep trying
                    spdlog::error("connect failed, reboot..");

                    // Reboot
                    delay(300);
                    esp_restart();
                    delay(10000);
                }
            }

            if (is_connected)
                break;
        }
        ledcDetach(14);

        gpio_reset_pin((gpio_num_t)HAL_PIN_LED);
        gpio_set_direction((gpio_num_t)HAL_PIN_LED, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode((gpio_num_t)HAL_PIN_LED, GPIO_PULLUP_ONLY);
    }
    spdlog::info("connect ok, ip: {}", WiFi.localIP().toString().c_str());

    // Mark to wait at next time
    if (HAL::GetSystemConfig().waitApFirst != "yes")
    {
        spdlog::info("start 10s ap waiting from next reboot");
        HAL::GetSystemConfig().waitApFirst = "yes";
        HAL::SaveSystemConfig();
    }

    // // Init sd card
    // HAL::SdCardInit(false);

    // Start poster
    start_poster_task();
}
