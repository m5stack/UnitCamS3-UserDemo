/**
 * @file hal_fs.cpp
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
#include <LittleFS.h>
#include <ArduinoJson.h>

static const char* _config_file_path = "/config.json";

void HAL_UnitCamS3_5MP::_fs_init()
{
    spdlog::info("fs init");

    if (!LittleFS.begin(true))
    {
        popFatalError("littlefs init failed");
    }

    loadSystemConfig();
    _log_out_system_config();
}

void HAL_UnitCamS3_5MP::loadSystemConfig()
{
    spdlog::info("load system config");

    // Check exist
    if (!LittleFS.exists(_config_file_path))
    {
        spdlog::warn("no config file, create default");
        saveSystemConfig();
        return;
    }

    // Try open
    File file = LittleFS.open(_config_file_path, "r");
    if (!file)
    {
        spdlog::error("open {} failed", _config_file_path);
        return;
    }

    // Parse json
    JsonDocument doc;
    deserializeJson(doc, file);

    // Copy into buffer
    _data.config.wifiSsid = doc["wifiSsid"].as<std::string>();
    _data.config.wifiPass = doc["wifiPass"].as<std::string>();
    _data.config.startPoster = doc["startPoster"].as<std::string>();
    _data.config.waitApFirst = doc["waitApFirst"].as<std::string>();
    _data.config.nickname = doc["nickname"].as<std::string>();
    _data.config.postInterval = doc["postInterval"];
    _data.config.timeZone = doc["timeZone"].as<std::string>();

    file.close();
}

void HAL_UnitCamS3_5MP::saveSystemConfig()
{
    spdlog::info("save system config");

    // Try open
    File file = LittleFS.open(_config_file_path, "w");
    if (!file)
    {
        spdlog::error("open {} failed", _config_file_path);
        return;
    }

    // Parse json
    JsonDocument doc;

    doc["wifiSsid"] = _data.config.wifiSsid;
    doc["wifiPass"] = _data.config.wifiPass;
    doc["startPoster"] = _data.config.startPoster;
    doc["waitApFirst"] = _data.config.waitApFirst;
    doc["nickname"] = _data.config.nickname;
    doc["postInterval"] = _data.config.postInterval;
    doc["timeZone"] = _data.config.timeZone;

    serializeJson(doc, file);

    file.close();
}

void HAL_UnitCamS3_5MP::_log_out_system_config()
{
    spdlog::info("system config:");
    printf("wifi ssid: %s\nwifi password: %s\nstart poster: %s\nwait ap first: %s\nnickname: %s\npost interval: %d\ntime zone: "
           "%s\n",
           _data.config.wifiSsid.c_str(),
           _data.config.wifiPass.c_str(),
           _data.config.startPoster.c_str(),
           _data.config.waitApFirst.c_str(),
           _data.config.nickname.c_str(),
           _data.config.postInterval,
           _data.config.timeZone.c_str());
}
