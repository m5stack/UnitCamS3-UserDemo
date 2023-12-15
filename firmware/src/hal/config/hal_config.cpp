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
#include <SD.h>
#include <FS.h>


using namespace HAL;


static const char* _config_file_path = "/config.json";

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
    doc["timeZone"] = cfg.time_zone;
    doc["startShooter"] = cfg.start_shooter;

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
    _data.config.time_zone = doc["timeZone"].as<String>();
    _data.config.start_shooter = doc["startShooter"].as<String>();

    file.close();

    return _data.config;
}


hal::Config_t hal::getDefaultConfig()
{
    Config_t ret;
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
        // Default config 
        setConfig(_data.config);
    }
    else 
    {
        spdlog::info("{} already exsit", _config_file_path);
    }

    // Sync config
    getConfig();

    // Hijack config if valid 
    if (_check_sd_card_config_hijack())
    {
        // Over write internal config 
        setConfig(_data.config);
    }

    printConfig();
}


bool hal::_check_sd_card_config_hijack()
{
    spdlog::info("try load config from sd..");

    // Check sd card 
    sdCardInit(true);
    if (!_data.is_sd_card_valid)
        return false;

    // Check config exist 
    if (!SD.exists(_config_file_path))
    {
        spdlog::info("{} not exist", _config_file_path);
        return false;
    }

    // Try open 
    File file = SD.open(_config_file_path, "r");
    if (!file)
    {
        spdlog::error("open {} failed", _config_file_path);
        return false;
    }

    // Parse json 
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, file);

    // Copy 
    spdlog::info("copy config from sd..");
    if (!doc["wifiSsid"].isNull())
        _data.config.wifi_ssid = doc["wifiSsid"].as<String>();
    if (!doc["wifiPass"].isNull())
        _data.config.wifi_password = doc["wifiPass"].as<String>();
    if (!doc["nickname"].isNull())
        _data.config.nickname = doc["nickname"].as<String>();
    if (!doc["postInterval"].isNull())
        _data.config.post_interval = doc["postInterval"];
    if (!doc["timeZone"].isNull())
        _data.config.time_zone = doc["timeZone"].as<String>();
    
    file.close();
    sdCardDeinit();
    return true;
}


void hal::printConfig()
{
    spdlog::info("config:");
    printf("wifi ssid: %s\n", _data.config.wifi_ssid.c_str());
    printf("wifi pass: %s\n", _data.config.wifi_password.c_str());
    printf("nickname: %s\n", _data.config.nickname.c_str());
    printf("post interval: %d\n", _data.config.post_interval);
    printf("time zone: %s\n", _data.config.time_zone.c_str());
    printf("wait ap first: %s\n", _data.config.wait_ap_first.c_str());
    printf("start poster: %s\n", _data.config.start_poster.c_str());
    printf("start interval shooter: %s\n", _data.config.start_shooter.c_str());
}
