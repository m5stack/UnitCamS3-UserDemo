/**
 * @file api_cam.h
 * @author Forairaaaaa
 * @brief https://gist.github.com/me-no-dev/d34fba51a8f059ac559bf62002e61aa3
 * @version 0.1
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <ESPAsyncWebServer.h>
// #include <WebServer.h>

void sendBMP(AsyncWebServerRequest *request);
void sendJpg(AsyncWebServerRequest *request);
void streamJpg(AsyncWebServerRequest *request);
void getCameraStatus(AsyncWebServerRequest *request);
void setCameraVar(AsyncWebServerRequest *request);

void load_cam_apis(AsyncWebServer& server);
