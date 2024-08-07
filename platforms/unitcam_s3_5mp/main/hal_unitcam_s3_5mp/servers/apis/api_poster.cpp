/**
 * @file api_system.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-08-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "apis.h"
#include <mooncake.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <hal/hal.h>
#include <LittleFS.h>
#include "utils/ezdata_image_poster.hpp"
#include "utils/status_led/status_led.h"
#include "apps/utils/system/inputs/inputs.h"
#include <esp_wifi.h>

using namespace SYSTEM::INPUTS;

static void _task_image_poster(void* param)
{
    uint32_t post_time_count = 0;
    int post_count_down = HAL::GetSystemConfig().postInterval;
    bool start_post = true;

    // Get mac
    std::string mac;
    {
        uint8_t mac_buffer[6];
        char macStr[18] = {0};
        esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac_buffer);
        sprintf(macStr,
                "%02X%02X%02X%02X%02X%02X",
                mac_buffer[0],
                mac_buffer[1],
                mac_buffer[2],
                mac_buffer[3],
                mac_buffer[4],
                mac_buffer[5]);
        mac = macStr;
    }
    spdlog::info("get mac: {}", mac);

    while (1)
    {
        delay(100);

        // Count down every seconds
        if (millis() - post_time_count > 1000)
        {
            if (start_post)
            {
                start_post = false;

                // Update led state
                StatusLed::SetState(led_state_posting);

                // Start posting
                spdlog::info("start posting..");
                if (!ezdata_image_poster(
                        mac, HAL::GetSystemConfig().nickname, HAL::GetSystemConfig().timeZone, [](camera_fb_t* frameBuffer) {
                            // Save on every captured
                            if (HAL::IsSdCardVaild())
                                HAL::SaveImage(frameBuffer->buf, frameBuffer->len);
                        }))
                {
                    // If failed
                    spdlog::error("post failed, try reboot..");
                    // Pass AP waiting
                    HAL::GetSystemConfig().waitApFirst = false;
                    HAL::SaveSystemConfig();
                    // Go
                    delay(1000);
                    esp_restart();
                }
                spdlog::info("done, wait next in {}s..", HAL::GetSystemConfig().postInterval);

                // Update led state
                StatusLed::SetState(led_state_waiting);

                // Reset count down
                post_count_down = HAL::GetSystemConfig().postInterval;
            }

            // Count down
            post_count_down--;
            if (post_count_down <= 0)
                start_post = true;

            // spdlog::info("post count down: {}", post_count_down);
            post_time_count = millis();
        }

        // If button 0 pressed
        Button::Update();
        if (Button::A()->wasClicked())
        {
            spdlog::info("g0 pressed, reset");

            // Reset
            CONFIG::SystemConfig_t default_config;
            HAL::GetSystemConfig() = default_config;
            HAL::GetSystemConfig().wifiSsid = "";
            HAL::GetSystemConfig().wifiPass = "";
            HAL::SaveSystemConfig();

            delay(300);
            esp_restart();
            delay(10000);
        }
    }

    vTaskDelete(NULL);
}

static void _task_start_poster(void* param)
{
    delay(500);
    spdlog::info("start poster");
    HAL::GetSystemConfig().startPoster = "yes";
    HAL::GetSystemConfig().waitApFirst = "no";
    HAL::SaveSystemConfig();

    delay(300);
    esp_restart();
    delay(10000);
    vTaskDelete(NULL);
}

void startPoster(AsyncWebServerRequest* request)
{
    xTaskCreate(_task_start_poster, "reboot", 4000, NULL, 15, NULL);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}

void load_poster_apis(AsyncWebServer& server) { server.on("/api/v1/start_poster", HTTP_GET, startPoster); }

void start_poster_task()
{
    // Start status led
    StatusLed::Start();

    // Create image poster task
    xTaskCreate(_task_image_poster, "poster", 4000, NULL, 5, NULL);
}
