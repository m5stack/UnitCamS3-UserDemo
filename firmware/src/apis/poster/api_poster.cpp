/**
 * @file api_poster.cpp
 * @author Forairaaaaa
 * @brief ref: https://randomnerdtutorials.com/esp32-cam-post-image-photo-server/
 * @version 0.1
 * @date 2023-11-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include "Arduino.h"
#include "api_poster.h"
#include "../../hal/hal.h"
#include <LittleFS.h>
#include "ezdata_image_poster.hpp"


// Led State -------------------------------------
static SemaphoreHandle_t _semaphore_led_state = NULL;

enum LedState_t
{
    led_state_waiting = 0,
    led_state_posting,
};
static LedState_t _led_state = led_state_waiting;

LedState_t _get_led_state()
{
    LedState_t ret = led_state_waiting;
    xSemaphoreTake(_semaphore_led_state, portMAX_DELAY);
    ret = _led_state;
    xSemaphoreGive(_semaphore_led_state);
    return ret;
}

void _set_led_state(LedState_t cfg)
{
    xSemaphoreTake(_semaphore_led_state, portMAX_DELAY);
    _led_state = cfg;
    xSemaphoreGive(_semaphore_led_state);
}
// Led State -------------------------------------


class PostTester
{
private:
    uint64_t _post_count;
public:
    PostTester() :
        _post_count(0)
        {}
    inline String getPostCount() 
    {
        spdlog::info("---------------------------");
        spdlog::info("post tester: count {}", _post_count);
        spdlog::info("free heap internal: {}", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
        // spdlog::info("free heap psram: {}", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
        // spdlog::info("free block internal: {}", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
        // spdlog::info("free block psram: {}", heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
        _post_count++;
        return String(_post_count); 
    }
};
static PostTester _post_tester;


static void _task_image_poster(void* param)
{
    gpio_reset_pin(GPIO_NUM_0);
    pinMode(0, INPUT_PULLUP);

    uint32_t post_time_count = 0;
    int post_count_down = HAL::hal::GetHal()->getConfig().post_interval;
    bool start_post = true;

    // Get path with mac 
    String server_path;
    {
        // Get mac 
        uint8_t mac[6];
        char macStr[18] = { 0 };
        if (WiFiGenericClass::getMode() == WIFI_MODE_NULL)
            esp_read_mac(mac, ESP_MAC_WIFI_STA);
        else 
            esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac);
        sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        spdlog::info("get mac: {}", macStr);

        server_path = "/api/v2/";
        server_path += macStr;
        server_path += "/uploadMacFile";
    }

    
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
                _set_led_state(led_state_posting);

                // Start posting 
                spdlog::info("start posting..");
                if (!ezdata_image_poster(
                    "ezdata2.m5stack.com", 
                    80, 
                    server_path, 
                    HAL::hal::GetHal()->getConfig().nickname,
                    HAL::hal::GetHal()->getConfig().time_zone
                ))
                {
                    // If failed 
                    spdlog::error("post failed, try reboot..");
                    // Pass AP waiting 
                    auto cfg = HAL::hal::GetHal()->getConfig();
                    cfg.wait_ap_first = false;
                    HAL::hal::GetHal()->setConfig(cfg);
                    // Go 
                    delay(1000);
                    esp_restart();
                }
                spdlog::info("done, wait next..");

                // Update led state 
                _set_led_state(led_state_waiting);

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
                spdlog::info("quit poster mode");
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

static void _led_blink()
{
    HAL::hal::GetHal()->setLed(true);
    delay(50);
    HAL::hal::GetHal()->setLed(false);
}


static void _task_led(void* param)
{
    LedState_t led_state = led_state_waiting;
    uint32_t state_time_count = 0;
    uint32_t led_time_count = 0;

    while (1)
    {
        // Update state 
        if (millis() - state_time_count > 1000)
        {
            led_state = _get_led_state();
            state_time_count = millis();
        }
            
        // Update led 
        if (led_state == led_state_waiting)
        {
            if (millis() - led_time_count > 2000)
            {
                _led_blink();
                led_time_count = millis();
            }
        }
        else 
        {
            if (millis() - led_time_count > 500)
            {
                _led_blink();
                delay(50);
                _led_blink();
                led_time_count = millis();
            }
        }

        delay(100);
    }

    vTaskDelete(NULL);
}




static void _task_start_poster(void* param)
{
    delay(500);
    spdlog::info("start poster");
    auto config = HAL::hal::GetHal()->getConfig();
    config.start_poster = "yes";
    config.wait_ap_first = "no";
    HAL::hal::GetHal()->setConfig(config);

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


void load_poster_apis(AsyncWebServer& server)
{
    server.on("/api/v1/start_poster", HTTP_GET, startPoster);
}

void start_poster_task()
{
    /* Create a mutex type semaphore. */
    _semaphore_led_state = xSemaphoreCreateMutex();

    // Create image poster task
    xTaskCreate(_task_image_poster, "poster", 4000, NULL, 5, NULL);
    // Create led task 
    xTaskCreate(_task_led, "led", 2000, NULL, 4, NULL);
}
