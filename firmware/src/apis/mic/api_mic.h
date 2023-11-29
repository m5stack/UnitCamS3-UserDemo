/**
 * @file api_mic.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <ESPAsyncWebServer.h>
// #include <WebServer.h>

void startMicRecording(AsyncWebServerRequest* request);
void isMicRecording(AsyncWebServerRequest* request);

void load_mic_apis(AsyncWebServer& server);
