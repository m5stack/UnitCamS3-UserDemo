/**
 * @file api_shooter.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <ESPAsyncWebServer.h>

void load_shooter_apis(AsyncWebServer& server);
void start_shooter_task();
