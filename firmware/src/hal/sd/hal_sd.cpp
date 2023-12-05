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


struct ImageSavePath_t 
{
public:
    const String path = "/captured";
    const String prefix = "img_";
    const String extension  = ".jpeg";

private:
    String _file_name_buffer;
    uint32_t _image_num;

public:
    ImageSavePath_t() :
        _image_num(0)
        {}

    void setImageNum(uint32_t imgNum) { _image_num = imgNum; }
    uint32_t getImageNum() { return _image_num; }
    void nextImage() { _image_num++; }

    const String& getSavePath()
    {
        _file_name_buffer = path;
        _file_name_buffer += "/";
        _file_name_buffer += prefix;
        _file_name_buffer += _image_num;
        _file_name_buffer += extension;

        spdlog::info("get save path: {}", _file_name_buffer.c_str());
        return _file_name_buffer;
    }
};
static ImageSavePath_t _img_save_path;


bool hal::sdCardInit()
{
    spdlog::info("try init sd card..");
    _data.is_sd_card_valid = false;
    
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
        return false;
    }

    spdlog::info("sd card init ok");
    spdlog::info("size: {}G", SD.cardSize() / 1073741824);
    

    // Check path 
    if (!SD.exists(_img_save_path.path))
    {
        spdlog::info("no path: {}, try create..", _img_save_path.path);
        if (!SD.mkdir(_img_save_path.path))
        {
            spdlog::error("create failed!");
            return false;
        }
    }

    // Open path and iterate file names 
    File dir = SD.open(_img_save_path.path, "r");
    uint32_t image_num = 0;
    while (1) 
    {
        File file = dir.openNextFile();
        if (!file) 
        {
            image_num++;
            break;
        }

        // Get image num from file name 
        String name = file.name();
        image_num = name.substring(name.indexOf(_img_save_path.prefix) + _img_save_path.prefix.length(), name.indexOf(_img_save_path.extension)).toInt();
    }

    // Save image num 
    _img_save_path.setImageNum(image_num);
    spdlog::info("get image num: {}", _img_save_path.getImageNum());


    spdlog::info("done");
    _data.is_sd_card_valid = true;
    return true;
}


void hal::saveImage(uint8_t* img, size_t size)
{   
    spdlog::info("try save image at {} ..", _img_save_path.getSavePath());

    // Save 
    File file = SD.open(_img_save_path.getSavePath(), "w", true);
    file.write(img, size);
    file.close();

    spdlog::info("done");
    _img_save_path.nextImage();
}
