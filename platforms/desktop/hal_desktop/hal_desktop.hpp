/**
 * @file hal_desktop.hpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <cstdint>
#include <mooncake.h>
#include <M5GFX.h>
#include "LGFX_AutoDetect_sdl.hpp"
#include "spdlog/spdlog.h"
#include <hal/hal.h>
#include <cmath>
#include <string>
#include <vector>
#include <random>

class HAL_Desktop : public HAL
{
private:
    int _screenWidth;
    int _screenHeight;
    bool _factory_test;

public:
    std::string type() override
    {
        return "AtomS3R";
        // if (_factory_test)
        //     return "AtomS3R";
        // return "Desktop";
    }

    HAL_Desktop(int screenWidth = 240, int screenHeight = 240, bool factoryTest = false)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;
        _factory_test = factoryTest;
    }

    void init() override
    {
        // Display
        _data.display = new LGFX(_screenWidth, _screenHeight);
        _data.display->init();

        // Canvas
        _data.canvas = new LGFX_SpriteFx(_data.display);
        _data.canvas->createSprite(_data.display->width(), _data.display->height());

        if (_factory_test)
        {
            _data.unit_oled = new LGFX(128, 64);
            _data.unit_oled->init();
            _data.unit_oled->setColorDepth(1);
            _data.unit_oled->fillScreen(TFT_WHITE);
        }

        // Some pc window pop up slower?
        lgfx::delay(1500);

        // this->popWarning("404 not found\nasdasd asdfasf");
        // this->popSuccess("404 not found\nasdasd asdfasf");
        // this->popFatalError("404 not found\nasdasd asdfasf");

        _data.config.wifiSsid = "114514";
        _data.config.wifiPass= "1919810";

        // Add key mapping
        auto panel = (lgfx::Panel_sdl*)_data.display->getPanel();
        panel->addKeyCodeMapping(SDLK_q, 0);
        panel->addKeyCodeMapping(SDLK_w, 1);
        // panel->addKeyCodeMapping(SDLK_e, 2);
        // panel->addKeyCodeMapping(SDLK_r, 3);
        // panel->addKeyCodeMapping(SDLK_f, 4);
    }

    void canvasUpdate() override { GetCanvas()->pushSprite(0, 0); }

    bool getButton(GAMEPAD::GamePadButton_t button) override
    {
        if (button == GAMEPAD::BTN_A)
            return !lgfx::gpio_in(0);
        else if (button == GAMEPAD::BTN_B)
            return !lgfx::gpio_in(1);
        // else if (button == GAMEPAD::BTN_START)
        //     return !lgfx::gpio_in(2);

        return false;
    }

    // /* -------------------------------------------------------------------------- */
    // /*                                   Encoder                                  */
    // /* -------------------------------------------------------------------------- */
    // // Simulate encoder by mouse drag
    // int encoder_count = 0;
    // int last_encoder_count = 0;
    // int touch_start_x = 0;
    // int touch_last_x = 0;
    // bool is_touching = false;
    // int getEncoderCount() override
    // {
    //     if (isTouching())
    //     {
    //         if (!is_touching)
    //         {
    //             is_touching = true;
    //             touch_start_x = getTouchPoint().x;
    //             return encoder_count;
    //         }

    //         if (getTouchPoint().x != touch_last_x)
    //         {
    //             // spdlog::info("{} {} {}", touch_start_x, getTouchPoint().x, encoder_count);
    //             encoder_count = (getTouchPoint().x - touch_start_x) / 20 + last_encoder_count;
    //         }
    //         touch_last_x = getTouchPoint().x;
    //     }
    //     else if (is_touching)
    //     {
    //         is_touching = false;
    //         last_encoder_count = encoder_count;
    //     }

    //     // // Reverse
    //     // return -encoder_count;
    //     // // return encoder_count;

    //     if (_data.config.reverseEncoder)
    //         return encoder_count;
    //     return -encoder_count;
    // }

    // void resetEncoderCount(int value) override { encoder_count = value; }

    bool isTouching() override
    {
        lgfx::touch_point_t tp;
        _data.display->getTouch(&tp);
        // spdlog::info("{} {} {}", tp.x, tp.y, tp.size);
        return tp.size != 0;
    }

    TOUCH::Point_t getTouchPoint() override
    {
        lgfx::touch_point_t tp;
        _data.display->getTouch(&tp);
        return {tp.x, tp.y};
    }

    float generateRandomNumber(float v1, float v2)
    {
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> distribution(v1, v2);
        return distribution(generator);
    }

    bool connectWifi(OnLogPageRenderCallback_t onLogPageRender, bool reconnect) override
    {
        onLogPageRender(" connecting wifi..\n", true, true);
        delay(300);
        return true;
    }

    OTA_UPGRADE::OtaInfo_t getLatestFirmwareInfoViaOta(OnLogPageRenderCallback_t onLogPageRender) override
    {
        OTA_UPGRADE::OtaInfo_t info;
        info.firmwareUrl = "www.114514.com";
        info.latestVersion = "V6.6.6";
        info.upgradeAvailable = true;
        return info;
    }

    bool upgradeFirmwareViaOta(OnLogPageRenderCallback_t onLogPageRender, const std::string& firmwareUrl) override
    {
        onLogPageRender(" connecting wifi..\n", true, false);
        delay(200);

        onLogPageRender(" upgrading..\n", true, false);
        delay(200);

        for (int i = 0; i < 100; i += 5)
        {
            std::string log = "<PB>";
            log += std::to_string(i);
            log += "\n";
            onLogPageRender(log, true, false);
            delay(100);
        }

        onLogPageRender(" done\n", true, true);
        delay(200);

        popSuccess("Upgrade done");
        return true;
    }
};
