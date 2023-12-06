/**
 * @file status_led.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "status_led.h"
#include "../../hal/hal.h"


static SemaphoreHandle_t _semaphore_led_state = NULL;
static LedState_t _led_state = led_state_waiting;


LedState_t StatusLed::getState()
{
    LedState_t ret = led_state_waiting;
    xSemaphoreTake(_semaphore_led_state, portMAX_DELAY);
    ret = _led_state;
    xSemaphoreGive(_semaphore_led_state);
    return ret;
}


void StatusLed::setState(LedState_t state)
{
    xSemaphoreTake(_semaphore_led_state, portMAX_DELAY);
    _led_state = state;
    xSemaphoreGive(_semaphore_led_state);
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
            led_state = StatusLed::getState();
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


void StatusLed::start()
{
    /* Create a mutex type semaphore. */
    _semaphore_led_state = xSemaphoreCreateMutex();
    // Create led task 
    xTaskCreate(_task_led, "led", 2000, NULL, 4, NULL);
}
