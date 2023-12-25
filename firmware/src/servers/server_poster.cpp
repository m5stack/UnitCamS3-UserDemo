/**
 * @file server_poster.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "servers.h"
#include <mooncake.h>
#include <Arduino.h>
#include "hal/hal.h"

#include <FS.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

#include "apis/camera/api_cam.h"
#include "apis/system/api_system.h"
#include "apis/mic/api_mic.h"
#include "apis/poster/api_poster.h"



void UserDemoServers::start_poster_server()
{
    Serial.begin(115200);

    spdlog::info("start poster server");
    auto config = HAL::hal::GetHal()->getConfig();


    // If need to wait ap connection for 10s
    if (config.wait_ap_first == "yes")
    {
        spdlog::info("start wait ap");
        start_ap_server();

        uint32_t ap_wait_time_cout = millis();
        bool led_state = true;
        while (millis() - ap_wait_time_cout < 10000)
        {
            // If get client 
            if (WiFi.softAPgetStationNum() != 0)
            {
                // Hold Ap mode 
                spdlog::info("get client, hold ap mode");
                HAL::hal::GetHal()->setLed(true);
                while (WiFi.softAPgetStationNum() != 0)
                    delay(2000);

                // If no config setting, reset counting
                ap_wait_time_cout = millis();
            }

            delay(500);
            led_state = !led_state;
            HAL::hal::GetHal()->setLed(led_state);
        }
        
        spdlog::info("no client, go on poster mode");
        stop_ap_server();
    }



    // Try connect wifi 
    spdlog::info("try conncet:\n{}\n{}", config.wifi_ssid.c_str(), config.wifi_password.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.wifi_ssid, config.wifi_password);

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
        ledcAttachPin(14, 1);
        ledcSetup(1, 12000, 8);
        uint32_t time_count = millis();
        bool is_connected = false;
        while (1) 
        {
            for (int i = 0; i < 255; i++)
            {
                ledcWrite(1, i);
                delay(5);
            }

            for (int i = 255; i > 0; i--)
            {
                ledcWrite(1, i);
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
        ledcDetachPin(14);
        gpio_reset_pin(GPIO_NUM_14);
        pinMode(14, OUTPUT);
    }
    spdlog::info("connect ok, ip: {}", WiFi.localIP().toString().c_str());
    

    // Mark to wait at next time 
    if (config.wait_ap_first != "yes")
    {
        spdlog::info("start 10s ap waiting from next reboot");
        config.wait_ap_first = "yes";
        HAL::hal::GetHal()->setConfig(config);
    }
    

    // Init sd card 
    HAL::hal::GetHal()->sdCardInit();

    // Start poster 
    start_poster_task();
}

