/**
 * @file types.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <smooth_ui_toolkit.h>
#include <thread>
#include <mutex>

/* -------------------------------------------------------------------------- */
/*                               Gamepad button                               */
/* -------------------------------------------------------------------------- */
namespace GAMEPAD
{
    enum GamePadButton_t
    {
        BTN_START = 0,
        BTN_SELECT,
        BTN_UP,
        BTN_LEFT,
        BTN_RIGHT,
        BTN_DOWN,
        BTN_X,
        BTN_Y,
        BTN_A,
        BTN_B,
        BTN_LEFT_STICK,
        GAMEPAD_BUTTON_NUM,
    };
} // namespace GAMEPAD

/* -------------------------------------------------------------------------- */
/*                                     IMU                                    */
/* -------------------------------------------------------------------------- */
namespace IMU
{
    struct ImuData_t
    {
        float accelX;
        float accelY;
        float accelZ;

        float gyroX;
        float gyroY;
        float gyroZ;

        float magX;
        float magY;
        float magZ;
    };
} // namespace IMU

/* -------------------------------------------------------------------------- */
/*                                  Touchpad                                  */
/* -------------------------------------------------------------------------- */
namespace TOUCH
{
    /**
     * @brief Touch point
     *
     */
    struct Point_t
    {
        int x;
        int y;
    };
} // namespace TOUCH

/* -------------------------------------------------------------------------- */
/*                                 Locale code                                */
/* -------------------------------------------------------------------------- */
enum LocaleCode_t
{
    locale_code_en = 0,
    locale_code_cn,
    locale_code_jp,
};

/* -------------------------------------------------------------------------- */
/*                                System config                               */
/* -------------------------------------------------------------------------- */
namespace CONFIG
{
    // Default config
    struct SystemConfig_t
    {
        std::string wifiSsid = "";
        std::string wifiPass = "";
        std::string startPoster = "no";
        std::string waitApFirst = "no";
        std::string nickname = "UnitCamS3";
        std::string timeZone = "GMT+0";
        int postInterval = 5;
    };
} // namespace CONFIG

/* -------------------------------------------------------------------------- */
/*                                  OTA info                                  */
/* -------------------------------------------------------------------------- */
namespace OTA_UPGRADE
{
    struct OtaInfo_t
    {
        bool upgradeAvailable = false;
        bool getInfoFailed = false;
        std::string latestVersion;
        std::string firmwareUrl;
    };
}; // namespace OTA_UPGRADE

/* -------------------------------------------------------------------------- */
/*                                Factory Test                                */
/* -------------------------------------------------------------------------- */
namespace FACTORY_TEST
{
    class WifiTestResult_t
    {
    public:
        struct Data_t
        {
            std::mutex mutex;
            bool is_done = false;
            bool is_passed = false;
            int32_t best_rssi = 0;
            std::string test_result = "";
            std::string target_ssid = "";
            std::string target_password = "";
        };
        Data_t _data;

        Data_t& Borrow()
        {
            _data.mutex.lock();
            return _data;
        }

        void Return() { _data.mutex.unlock(); }

        Data_t& Data() { return _data; }
    };

}; // namespace FACTORY_TEST

/* -------------------------------------------------------------------------- */
/*                                    MISC                                    */
/* -------------------------------------------------------------------------- */
typedef std::function<void(const std::string& log, bool pushScreen, bool clear)> OnLogPageRenderCallback_t;
