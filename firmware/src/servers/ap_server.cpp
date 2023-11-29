/**
 * @file ap_server.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "servers.h"
#include <mooncake.h>
#include <Arduino.h>
#include "hal/hal.h"

#include <FS.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <SPIFFSEditor.h>

#include "apis/camera/api_cam.h"
#include "apis/system/api_system.h"
#include "apis/mic/api_mic.h"
#include "apis/poster/api_poster.h"


static AsyncWebServer* _server = nullptr;

void UserDemoServers::start_ap_server()
{
    delay(200);
    Serial.begin(115200);
    Serial.printf("start ap server\n");


    // Create web server 
    _server = new AsyncWebServer(80);


    // Start open ap 
    // uint64_t chipid = ESP.getEfuseMac();
    // String ap_ssid  = "UNIT-CAM-S3-" + String((uint32_t)(chipid >> 32), HEX);
    String ap_ssid = "UnitCamS3-WiFi";
    // String ap_pass = "12345678";

    // WiFi.softAP(ap_ssid);
    WiFi.softAP(ap_ssid, emptyString, 1, 0, 1, false);
    // WiFi.softAP(ap_ssid, ap_pass);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);


    // Load apis 
    load_cam_apis(*_server);
    load_system_apis(*_server);
    load_mic_apis(*_server);
    load_poster_apis(*_server);

    // Set file system support 
    _server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    _server->onNotFound([](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/404.html");
    });


    // Start server
    _server->begin();

    // Light up 
    HAL::hal::GetHal()->setLed(true);
}


void UserDemoServers::stop_ap_server()
{
    spdlog::info("stop ap server");

    delete _server;
    delay(200);

    WiFi.softAPdisconnect();
    WiFi.disconnect();
}
