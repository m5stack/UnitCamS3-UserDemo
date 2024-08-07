/**
 * @file apis.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2024-08-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <ESPAsyncWebServer.h>

void load_cam_apis(AsyncWebServer& server);
void load_mic_apis(AsyncWebServer& server);
void load_system_apis(AsyncWebServer& server);
void load_poster_apis(AsyncWebServer& server);
void start_poster_task();

