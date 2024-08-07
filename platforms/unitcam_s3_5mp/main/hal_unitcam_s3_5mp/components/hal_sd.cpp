/**
 * @file hal_sd.cpp
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
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <driver/gpio.h>

std::string HAL_UnitCamS3_5MP::getSdCardInfo()
{
    spdlog::info("try get sd card info");

    String card_info = "{\"info\":\"";

    spdlog::info("cs:{} miso:{} mosi:{} clk:{}", HAL_PIN_SD_CS, HAL_PIN_SD_MISO, HAL_PIN_SD_MOSI, HAL_PIN_SD_CLK);

    // Init spi
    // SPIClass* sd_spi = new SPIClass(HSPI);
    // sd_spi->begin(HAL_PIN_SD_CLK, HAL_PIN_SD_MISO, HAL_PIN_SD_MOSI, HAL_PIN_SD_CS);

    SPI.begin(HAL_PIN_SD_CLK, HAL_PIN_SD_MISO, HAL_PIN_SD_MOSI, HAL_PIN_SD_CS);

    // Better card compatibility?
    uint8_t pin_map[] = {HAL_PIN_SD_CLK, HAL_PIN_SD_MISO, HAL_PIN_SD_MOSI};
    for (auto& i : pin_map)
    {
        gpio_set_drive_capability((gpio_num_t)i, GPIO_DRIVE_CAP_3);
    }

    // Init sd
    bool ret = SD.begin(HAL_PIN_SD_CS, SPI, 10000000);
    if (!ret)
    {
        spdlog::error("sd begin failed");
        card_info += "SD Card Not Valid";
    }
    else
    {
        spdlog::info("sd begin ok");

        card_info += "Type: ";

        // Get card info
        uint8_t cardType = SD.cardType();
        if (cardType == CARD_MMC)
        {
            card_info += "MMC ";
            spdlog::info("MMC");
        }
        else if (cardType == CARD_SD)
        {
            card_info += "SDSC ";
            spdlog::info("SDSC");
        }
        else if (cardType == CARD_SDHC)
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
    // delete sd_spi;

    return card_info.c_str();
}

static bool _is_sd_card_vaild = false;

struct ImageSavePath_t
{
public:
    const String path = "/captured";
    const String prefix = "img_";
    const String extension = ".jpeg";

private:
    String _file_name_buffer;
    uint32_t _image_num;

public:
    ImageSavePath_t() : _image_num(0) {}

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

        // spdlog::info("get save path: {}", _file_name_buffer.c_str());
        return _file_name_buffer;
    }
};
static ImageSavePath_t _img_save_path;

bool HAL_UnitCamS3_5MP::sdCardInit(bool passImagePath)
{
    spdlog::info("try init sd card");

    spdlog::info("cs:{} miso:{} mosi:{} clk:{}", HAL_PIN_SD_CS, HAL_PIN_SD_MISO, HAL_PIN_SD_MOSI, HAL_PIN_SD_CLK);

    SPI.begin(HAL_PIN_SD_CLK, HAL_PIN_SD_MISO, HAL_PIN_SD_MOSI, HAL_PIN_SD_CS);

    // Better card compatibility?
    uint8_t pin_map[] = {HAL_PIN_SD_CLK, HAL_PIN_SD_MISO, HAL_PIN_SD_MOSI};
    for (auto& i : pin_map)
    {
        gpio_set_drive_capability((gpio_num_t)i, GPIO_DRIVE_CAP_3);
    }

    if (!SD.begin(HAL_PIN_SD_CS, SPI, 10000000))
    {
        spdlog::error("sd card init failed");
        _is_sd_card_vaild = false;
        return false;
    }

    spdlog::info("sd card init ok");
    spdlog::info("size: {}G", SD.cardSize() / 1073741824);
    _is_sd_card_vaild = true;

    // Path
    if (passImagePath)
    {
        spdlog::info("pass image path init, done");
        return true;
    }

    // Check path
    if (!SD.exists(_img_save_path.path))
    {
        spdlog::info("no path: {}, try create..", _img_save_path.path);
        if (!SD.mkdir(_img_save_path.path))
        {
            spdlog::error("create failed!");
            SD.end();
            return false;
        }
    }
    else
    {
        spdlog::info("{} alreay exist", _img_save_path.path.c_str());
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
        image_num = name.substring(name.indexOf(_img_save_path.prefix) + _img_save_path.prefix.length(),
                                   name.indexOf(_img_save_path.extension))
                        .toInt();
    }

    // Save image num
    _img_save_path.setImageNum(image_num);
    spdlog::info("get image num: {}", _img_save_path.getImageNum());

    spdlog::info("done");
    return true;
}

bool HAL_UnitCamS3_5MP::sdCardDeInit() { return false; }

bool HAL_UnitCamS3_5MP::isSdCardVaild() { return _is_sd_card_vaild; }

bool HAL_UnitCamS3_5MP::saveImage(uint8_t* img, size_t size)
{
    spdlog::info("try save image at {} ..", _img_save_path.getSavePath().c_str());

    // Save
    File file = SD.open(_img_save_path.getSavePath(), "w", true);
    if (file)
    {
        file.write(img, size);
        file.close();
    }
    else
    {
        spdlog::error("open {} failed!", _img_save_path.getSavePath());
        return false;
    }

    spdlog::info("done");
    _img_save_path.nextImage();
    return true;
}
