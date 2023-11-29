/**
 * @file hal_config.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include <Arduino.h>
#include "../hal.h"
#include <LittleFS.h>
#include <ArduinoJson.h>


using namespace HAL;


static const char* _config_file_path = "/config.json";
static const char* _default_config = "{\"wifiSsid\":\"\",\"wifiPass\":\"\",\"postInterval\":30,\"startPoster\":\"no\"}";


void hal::setConfig(hal::Config_t& cfg)
{
    // Try open 
    File file = LittleFS.open(_config_file_path, "w");
    if (!file)
    {
        spdlog::error("open {} failed", _config_file_path);
        return;
    }

    // Parse json 
    DynamicJsonDocument doc(1024);
    
    doc["wifiSsid"] = cfg.wifi_ssid;
    doc["wifiPass"] = cfg.wifi_password;
    doc["startPoster"] = cfg.start_poster;
    doc["waitApFirst"] = cfg.wait_ap_first;
    doc["nickname"] = cfg.nickname;
    doc["postInterval"] = cfg.post_interval;

    serializeJson(doc, file);

    file.close();
}


hal::Config_t hal::getConfig()
{
    // Try open 
    File file = LittleFS.open(_config_file_path, "r");
    if (!file)
    {
        spdlog::error("open {} failed", _config_file_path);
        return _data.config;
    }

    // Parse json 
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, file);

    // Copy into buffer 
    _data.config.wifi_ssid = doc["wifiSsid"].as<String>();
    _data.config.wifi_password = doc["wifiPass"].as<String>();
    _data.config.start_poster = doc["startPoster"].as<String>();
    _data.config.wait_ap_first = doc["waitApFirst"].as<String>();
    _data.config.nickname = doc["nickname"].as<String>();
    _data.config.post_interval = doc["postInterval"];

    file.close();

    return _data.config;
}


hal::Config_t hal::getDefaultConfig()
{
    Config_t ret;
    ret.wifi_ssid = "";
    ret.wifi_password = "";
    ret.start_poster = "no";
    ret.wait_ap_first = "no";
    ret.nickname = "UnitCamS3";
    // ret.post_interval = 30;
    ret.post_interval = 5;
    return ret;
}


void hal::_config_init()
{
    // File system 
    if (!LittleFS.begin(true)) 
    {
        while (1)
        {
            spdlog::info("file system init failed");
            delay(1000);
        }
    }

    spdlog::info("config init");

    // Check exist 
    if (!LittleFS.exists(_config_file_path))
    {
        // spdlog::error("{} not exist, creating", _config_file_path);
        // File new_file = LittleFS.open(_config_file_path, "w", true);
        // new_file.printf("%s", _default_config);
        // new_file.close();

        // Default config 
        setConfig(_data.config);
    }
    else 
    {
        spdlog::error("{} already exsit", _config_file_path);
    }

    // Fetch config
    getConfig();
    printConfig();
}


void hal::printConfig()
{
    spdlog::info("config:");
    printf("wifi ssid: %s\nwifi password: %s\nstart poster: %s\nwait ap first: %s\nnickname: %s\npost interval: %d\n", 
        _data.config.wifi_ssid.c_str(), 
        _data.config.wifi_password.c_str(),
        _data.config.start_poster.c_str(),
        _data.config.wait_ap_first.c_str(),
        _data.config.nickname.c_str(),
        _data.config.post_interval
    );
}
