/**
 * @file api_system.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include "Arduino.h"
#include "api_system.h"
#include "../../hal/hal.h"
#include <SPI.h>
#include <SD.h>
#include <FS.h>



void getMac(AsyncWebServerRequest* request)
{
    String result = "{\"msg\":\"ok\",\"mac\":\"";
    result += WiFi.macAddress();
    result += "\"}";

    request->send(200, "application/json", result);
}


void getSdCardInfo(AsyncWebServerRequest* request)
{
    String card_info = "{\"info\":\"";


    auto sd_pins = HAL::hal::GetHal()->getSdCardPin();
    spdlog::info("cs:{} miso:{} mosi:{} clk:{}", sd_pins.cs, sd_pins.miso, sd_pins.mosi, sd_pins.clk);

    /// Init spi 
    SPIClass* sd_spi = new SPIClass(HSPI);
    sd_spi->begin(
        sd_pins.clk,
        sd_pins.miso,
        sd_pins.mosi,
        sd_pins.cs
    );

    // Better card compatibility? 
    uint8_t pin_map[] = {sd_pins.clk, sd_pins.miso, sd_pins.mosi};
    for (auto gpio : pin_map)
    {
        *(volatile uint32_t*) (GPIO_PIN_MUX_REG[gpio]) |= FUN_DRV_M;
        gpio_pulldown_dis((gpio_num_t)gpio);
        gpio_pullup_en((gpio_num_t)gpio);
    }


    // Init sd 
    bool ret = SD.begin(sd_pins.cs, *sd_spi, 10000000);
    if (!ret)
    {
        spdlog::error("sd.begin failed");
        card_info += "SD Card Not Valid";
    }
    else 
    {
        spdlog::info("sd.begin ok");

        card_info += "Type: ";

        // Get card info
        uint8_t cardType = SD.cardType();
        if (cardType == CARD_MMC)
        {
            card_info += "MMC ";
            spdlog::info("MMC");
        }
        else if(cardType == CARD_SD)
        {
            card_info += "SDSC ";
            spdlog::info("SDSC");
        }
        else if(cardType == CARD_SDHC)
        {
            card_info += "SDHC ";
            spdlog::info("SDHC");
        }

        card_info += " Size: ";

        card_info += SD.cardSize() / 1073741824;
        card_info += "G";
    }

    card_info += "\"}";
    spdlog::info("{}", card_info.c_str());

    // Free     
    SD.end();
    delete sd_spi;

    request->send(200, "application/json", card_info);
}


void ledOn(AsyncWebServerRequest* request)
{
    HAL::hal::GetHal()->setLed(true);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}


void ledOff(AsyncWebServerRequest* request)
{
    HAL::hal::GetHal()->setLed(false);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}


#include <FS.h>
#include <LittleFS.h>

void getConfig(AsyncWebServerRequest* request)
{
    request->send(LittleFS, "/config.json");
}


void resetConfig(AsyncWebServerRequest* request)
{
    auto cfg = HAL::hal::GetHal()->getDefaultConfig();
    HAL::hal::GetHal()->setConfig(cfg);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}


#include <ArduinoJson.h>
#include <AsyncJson.h>

void setConfig(AsyncWebServerRequest* request, JsonVariant& json)
{
    HAL::hal::Config_t config;

    // Copy configs 
    config.wifi_ssid = json["wifiSsid"].as<String>();
    config.wifi_password = json["wifiPass"].as<String>();
    config.start_poster = json["startPoster"].as<String>();
    config.post_interval = json["postInterval"];
    config.nickname = json["nickname"].as<String>();
    config.time_zone = json["timeZone"].as<String>();

    // Check nickname 
    if (config.nickname == "" || config.nickname == "null")
        config.nickname = "UnitCamS3";

    // Check bad configs 
    if (config.wifi_ssid == "" || config.wifi_password == "" || config.start_poster == "" || config.post_interval == 0 || config.time_zone == "")
    {
        request->send(500, "application/json", "{\"msg\":\"bad config\"}");
        return;
    }
    if (config.wifi_ssid == "null" || config.wifi_password == "null" || config.start_poster == "null" || config.time_zone == "null")
    {
        request->send(500, "application/json", "{\"msg\":\"bad config\"}");
        return;
    }

    // Store 
    HAL::hal::GetHal()->setConfig(config);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}


static std::array<std::string, 20> _wifi_list_buffer;

void getWifiList(AsyncWebServerRequest* request)
{
    // Scan 
    spdlog::info("start scan");
    int n = WiFi.scanNetworks();
    spdlog::info("done, num: {}", n);

    for (int i = 0; i < n; i++)
    {
        if (i >= _wifi_list_buffer.size())
            break;
        _wifi_list_buffer[i] = WiFi.SSID(i).c_str();
    }
    WiFi.scanDelete();

    // Json 
    DynamicJsonDocument doc(4096);
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
    server.on("/api/v1/led_off", HTTP_GET, ledOff);
    server.on("/api/v1/get_config", HTTP_GET, getConfig);
    AsyncCallbackJsonWebHandler* getConfigHandler = new AsyncCallbackJsonWebHandler("/api/v1/set_config", setConfig);
    server.addHandler(getConfigHandler);
    server.on("/api/v1/reset_config", HTTP_GET, resetConfig);
    server.on("/api/v1/get_wifi_list", HTTP_GET, getWifiList);
}

