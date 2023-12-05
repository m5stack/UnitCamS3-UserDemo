/**
 * @file hal_sd.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include <Arduino.h>
#include "../hal.h"
#include <SPI.h>
#include <SD.h>
#include <FS.h>


using namespace HAL;


bool hal::sdCardInit()
{
    spdlog::info("try init sd card..");
    
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
        spdlog::error("sd card init failed");
        _data.is_sd_card_valid = false;
        return false;
    }

    spdlog::info("sd card init ok");
    spdlog::info("size: {}G", SD.cardSize() / 1073741824);
    
    // Check path 
    


    return true;
}
