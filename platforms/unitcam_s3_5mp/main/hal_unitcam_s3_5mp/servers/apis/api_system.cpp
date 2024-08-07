/**
 * @file api_system.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-08-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "apis.h"
#include <mooncake.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <hal/hal.h>

void getMac(AsyncWebServerRequest* request)
{
    String result = "{\"msg\":\"ok\",\"mac\":\"";
    result += WiFi.macAddress();
    result += "\"}";

    request->send(200, "application/json", result);
}

void getSdCardInfo(AsyncWebServerRequest* request) { request->send(200, "application/json", HAL::GetSdCardInfo().c_str()); }

void ledOn(AsyncWebServerRequest* request)
{
    HAL::SetLedState(true);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}

void ledOff(AsyncWebServerRequest* request)
{
    HAL::SetLedState(false);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}

#include <FS.h>
#include <LittleFS.h>

void getConfig(AsyncWebServerRequest* request) { request->send(LittleFS, "/config.json"); }

void resetConfig(AsyncWebServerRequest* request)
{
    CONFIG::SystemConfig_t default_config;
    HAL::GetSystemConfig() = default_config;
    HAL::SaveSystemConfig();

    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}

#include <ArduinoJson.h>
#include <AsyncJson.h>

void setConfig(AsyncWebServerRequest* request, JsonVariant& json)
{
    CONFIG::SystemConfig_t config;

    // Copy configs
    config.wifiSsid = json["wifiSsid"].as<std::string>();
    config.wifiPass = json["wifiPass"].as<std::string>();
    config.startPoster = json["startPoster"].as<std::string>();
    config.postInterval = json["postInterval"];
    config.nickname = json["nickname"].as<std::string>();
    config.timeZone = json["timeZone"].as<std::string>();

    // Check nickname
    if (config.nickname == "" || config.nickname == "null")
        config.nickname = "UnitCamS3";

    // Check bad configs
    if (config.startPoster == "" || config.postInterval == 0 || config.timeZone == "")
    {
        request->send(500, "application/json", "{\"msg\":\"bad config\"}");
        return;
    }
    if (config.wifiSsid == "null" || config.wifiPass == "null" || config.startPoster == "null" || config.timeZone == "null")
    {
        request->send(500, "application/json", "{\"msg\":\"bad config\"}");
        return;
    }

    // Store
    HAL::GetSystemConfig() = config;
    HAL::SaveSystemConfig();

    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}

static std::array<std::string, 20> _wifi_list_buffer;

void getWifiList(AsyncWebServerRequest* request)
{
    // Scan
    spdlog::info("start scan");

    int n = WiFi.scanNetworks();
    // int n = WiFi.scanNetworks(true);
    // while (WiFi.scanComplete() == -1)
    // {
    //     delay(1000);
    // }

    spdlog::info("done, num: {}", n);

    for (int i = 0; i < n; i++)
    {
        if (i >= _wifi_list_buffer.size())
            break;
        _wifi_list_buffer[i] = WiFi.SSID(i).c_str();
    }
    WiFi.scanDelete();

    // Json
    JsonDocument doc;
    for (int i = 0; i < _wifi_list_buffer.size(); i++)
    {
        doc["wifiList"][i] = _wifi_list_buffer[i];
    }

    String json_buffer;
    serializeJson(doc, json_buffer);
    spdlog::info("result:\n {}", json_buffer.c_str());

    request->send(200, "application/json", json_buffer);
}

void load_system_apis(AsyncWebServer& server)
{
    server.on("/api/v1/get_mac", HTTP_GET, getMac);
    server.on("/api/v1/check_sdcard", HTTP_GET, getSdCardInfo);
    server.on("/api/v1/led_on", HTTP_GET, ledOn);
    server.on("/api/v1/led_off", HTTP_GET, ledOff);
    server.on("/api/v1/get_config", HTTP_GET, getConfig);
    AsyncCallbackJsonWebHandler* getConfigHandler = new AsyncCallbackJsonWebHandler("/api/v1/set_config", setConfig);
    server.addHandler(getConfigHandler);
    server.on("/api/v1/reset_config", HTTP_GET, resetConfig);
    server.on("/api/v1/get_wifi_list", HTTP_GET, getWifiList);
}
