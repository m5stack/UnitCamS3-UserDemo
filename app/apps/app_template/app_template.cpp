/**
 * @file app_template.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date <date></date>
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app_template.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;

// App name
const char* AppTemplate_Packer::getAppName() { return "AppTemplate"; }

// Theme color
constexpr static uint32_t _theme_color = 0xFFFFFF;
void* AppTemplate_Packer::getCustomData() { return (void*)(&_theme_color); }

// Icon
void* AppTemplate_Packer::getAppIcon() { return nullptr; }

// Like setup()...
void AppTemplate::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // Render full screen to theme color
    HAL::GetCanvas()->fillScreen(_theme_color);
    HAL::GetCanvas()->setTextScroll(true);
    HAL::GetCanvas()->setBaseColor(_theme_color);
    HAL::GetCanvas()->setTextColor(TFT_BLACK);
    HAL::GetCanvas()->setTextSize(1);
    HAL::CanvasUpdate();
}

// Like loop()...
void AppTemplate::onRunning()
{
    // Print hi in every 1s
    if (HAL::Millis() - _data.time_count > 1000)
    {
        spdlog::info("Hi");

        HAL::GetCanvas()->printf("> Hi\n");
        HAL::CanvasUpdate();

        _data.time_count = HAL::Millis();
    }

    // Update button state
    Button::Update();

    if (Button::A()->wasClicked())
    {
        spdlog::info("button a clicked");

        HAL::GetCanvas()->printf("> Btn A clicked\n");
        HAL::CanvasUpdate();
    }

    if (Button::A()->isHolding())
    {
        HAL::GetCanvas()->printf("> Bye\n");
        HAL::CanvasUpdate();

        destroyApp();
    }

    // ...
    // If you want to block here, feed the dog manually
    // HAL::FeedTheDog();
}

void AppTemplate::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // Release resources here..
    // delete 114514;
}
