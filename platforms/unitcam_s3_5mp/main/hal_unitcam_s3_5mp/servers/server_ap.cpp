/**
 * @file server_ap.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-08-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_unitcam_s3_5mp.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "apis/apis.h"
#include <assets/assets.h>

static AsyncWebServer* _server = nullptr;

void HAL_UnitCamS3_5MP::startApServer()
{
    spdlog::info("start ap server");

    // Create web server
    _server = new AsyncWebServer(80);

    // Start ap
    String ap_ssid = "UnitCamS3-WiFi";
    WiFi.enableSTA(true);
    WiFi.softAP(ap_ssid, emptyString, 1, 0, 1, false);
    spdlog::info("ap ip: {}", WiFi.softAPIP().toString().c_str());

    // Load page
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response =
            request->beginResponse_P(200, "text/html", AssetPool::GetImage().index_html_gz, 234419);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });

    // Load apis
    load_cam_apis(*_server);
    load_mic_apis(*_server);
    load_system_apis(*_server);
    load_poster_apis(*_server);

    // // Set file system support
    // server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    // server->onNotFound([](AsyncWebServerRequest* request) { request->send(LittleFS, "/404.html"); });

    // Start web server
    spdlog::info("start web server");
    _server->begin();

    // Light up
    HAL::SetLedState(true);
}

void HAL_UnitCamS3_5MP::stopApServer()
{
    spdlog::info("stop ap server");

    delete _server;
    delay(200);

    WiFi.softAPdisconnect();
    WiFi.disconnect();
}
