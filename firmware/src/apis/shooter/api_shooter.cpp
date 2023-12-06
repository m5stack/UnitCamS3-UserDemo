/**
 * @file api_shooter.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include "Arduino.h"
#include "api_shooter.h"
#include "../../hal/hal.h"
#include <esp_camera.h>
#include "../utils/led_status/status_led.h"


static void _task_image_shooter(void* param)
{
    gpio_reset_pin(GPIO_NUM_0);
    pinMode(0, INPUT_PULLUP);

    uint32_t post_time_count = 0;
    int post_count_down = HAL::hal::GetHal()->getConfig().post_interval;
    bool start_post = true;
    camera_fb_t* fb = NULL;

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
                StatusLed::setState(led_state_posting);

                // Start posting 
                spdlog::info("start shooting..");


                // Captrue 
                fb = esp_camera_fb_get();
                // If failed 
                if(!fb) 
                {
                    spdlog::error("camera capture failed, try reboot..");
                    // Pass AP waiting 
                    auto cfg = HAL::hal::GetHal()->getConfig();
                    cfg.wait_ap_first = false;
                    HAL::hal::GetHal()->setConfig(cfg);
                    // Go 
                    delay(1000);
                    esp_restart();
                }
                
                // Save 
                if (HAL::hal::GetHal()->isSdCardValid())
                    HAL::hal::GetHal()->saveImage(fb->buf, fb->len);
                else
                    spdlog::error("sd card not valid?");


                spdlog::info("done, wait next..");

                // Update led state 
                StatusLed::setState(led_state_waiting);

                // Reset count down
                post_count_down = HAL::hal::GetHal()->getConfig().post_interval;
            }

            // Count down 
            post_count_down--;
            if (post_count_down <= 0)
                start_post = true;
            
            // spdlog::info("post count down: {}", post_count_down);
            post_time_count = millis();
        }

        // If button 0 pressed 
        if (!digitalRead(0))
        {
            delay(50);
            if (!digitalRead(0))
            {
                spdlog::info("quit shooter mode");
                while (!digitalRead(0))
                    delay(100);

                // Reset 
                auto config = HAL::hal::GetHal()->getDefaultConfig();
                HAL::hal::GetHal()->setConfig(config);

                delay(300);
                esp_restart();
                delay(10000);
            }
        }
    }

    vTaskDelete(NULL);
}


static void _task_start_shooter(void* param)
{
    delay(500);
    spdlog::info("start shooter");
    auto config = HAL::hal::GetHal()->getConfig();
    config.start_shooter = "yes";
    config.wait_ap_first = "no";
    HAL::hal::GetHal()->setConfig(config);

    delay(300);
    esp_restart();
    delay(10000);
    vTaskDelete(NULL);
}


void startShooter(AsyncWebServerRequest* request)
{
    xTaskCreate(_task_start_shooter, "reboot", 4000, NULL, 15, NULL);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}


void load_shooter_apis(AsyncWebServer& server)
{
    server.on("/api/v1/start_shooter", HTTP_GET, startShooter);
}


void start_shooter_task()
{
    // Start status led 
    StatusLed::start();

    // Create image poster task
    xTaskCreate(_task_image_shooter, "shooter", 4000, NULL, 5, NULL);
}

