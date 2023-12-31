/**
 * @file hal.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "mic/Mic_Class.hpp"


/**
 * @brief 
 * 
 */
#define HAL_PIN_BTN_A       0

#define HAL_PIN_GROVE_SCL   19
#define HAL_PIN_GROVE_SDA   20

#define HAL_PIN_MIC_CLK     47
#define HAL_PIN_MIC_DATA    48

#define HAL_PIN_SD_CS       9
#define HAL_PIN_SD_MOSI     38
#define HAL_PIN_SD_CLK      39
#define HAL_PIN_SD_MISO     40


namespace HAL
{
    class hal
    {   
    public:
        struct Config_t
        {
            String wifi_ssid;
            String wifi_password;
            String start_poster = "no";
            String wait_ap_first = "no";
            String nickname = "UnitCamS3";
            String time_zone = "GMT+0";
            String start_shooter = "no";
            int post_interval = 5;
        };

    private:
        struct Data_t
        {
            Config_t config;
            bool is_wifi_config_vaild = false;
            bool is_sd_card_valid = false;
        };
        Data_t _data;
        
        void _cam_init();
        void _config_init();
        bool _check_sd_card_config_hijack();

    public:
        void init();

        // Mic 
        m5::Mic_Class* mic;

        // Sd card 
        struct SdCardPin_t
        {
            int cs = HAL_PIN_SD_CS;
            int mosi = HAL_PIN_SD_MOSI;
            int clk = HAL_PIN_SD_CLK;
            int miso = HAL_PIN_SD_MISO;
        };
        inline SdCardPin_t getSdCardPin() {
            SdCardPin_t pin;
            return pin;
        }

        void setLed(bool state);

        void setConfig(Config_t& cfg);
        Config_t getConfig();
        Config_t getDefaultConfig();
        void printConfig();
        
        // For poster mode usage 
        bool sdCardInit(bool passImagePath = false);
        void sdCardDeinit();
        inline bool isSdCardValid() { return _data.is_sd_card_valid; }
        bool saveImage(uint8_t* img, size_t size);


    private:
        static hal* _hal;        
    public:
        static hal* GetHal();
    };
}
