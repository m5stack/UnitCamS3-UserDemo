/**
 * @file main.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include <Arduino.h>
#include "hal/hal.h"
#include "servers/servers.h"


void setup() 
{
    // Init 
    HAL::hal::GetHal()->init();

    // Start server 
    if (HAL::hal::GetHal()->getConfig().start_poster == "yes")
        UserDemoServers::start_poster_server();
    else if (HAL::hal::GetHal()->getConfig().start_shooter == "yes")
        UserDemoServers::start_shooter_server();
    else 
        UserDemoServers::start_ap_server();
}


void loop() 
{
    delay(5000);
}
