/**
 * @file api_system.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <ESPAsyncWebServer.h>
// #include <WebServer.h>

void getMac(AsyncWebServerRequest* request);
void getSdCardInfo(AsyncWebServerRequest* request);
void ledOn(AsyncWebServerRequest* request);
void ledOff(AsyncWebServerRequest* request);

void load_system_apis(AsyncWebServer& server);
