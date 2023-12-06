/**
 * @file server_interval_shooting.cpp
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
#include <WiFi.h>
#include "../apis/shooter/api_shooter.h"


void UserDemoServers::start_shooter_server()
{
    spdlog::info("start interval shooting server");
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

    // Try init sd card 
    for (int i = 0; i < 5; i++)
    {
        // Init sd card 
        if (HAL::hal::GetHal()->sdCardInit())
            break;
        delay(200);
    }

    // If not valid 
    if (!HAL::hal::GetHal()->isSdCardValid())
    {
        spdlog::error("sd card init failed, return back to ap mode");
        config.start_shooter = "no";
        HAL::hal::GetHal()->setConfig(config);
        
        // Reboot 
        delay(300);
        esp_restart();
        delay(10000);
    }
    

    // Mark to wait at next time 
    if (config.wait_ap_first != "yes")
    {
        spdlog::info("start 10s ap waiting from next reboot");
        config.wait_ap_first = "yes";
        HAL::hal::GetHal()->setConfig(config);
    }


    // Start shooter 
    start_shooter_task();
}
