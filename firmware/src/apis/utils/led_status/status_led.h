/**
 * @file status_led.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <Arduino.h>


enum LedState_t
{
    led_state_waiting = 0,
    led_state_posting,
};


namespace StatusLed
{
    void start();
    LedState_t getState();
    void setState(LedState_t state); 
};
