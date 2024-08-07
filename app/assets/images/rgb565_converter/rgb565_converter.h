/**
 * @file rgb565_converter.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-07-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#ifdef ENABLE_PNG_CONVERTOR
#include <png.h>
#include <cstdint>
#include <cstddef>

// 返回值表示错误代码
enum class ImageConversionError
{
    Success = 0,
    FileNotFound,
    NotAPNG,
    MemoryError,
    InitError,
    ReadError,
    ConversionError,
};

class PNGImage
{
public:
    PNGImage(const char* filename);
    ~PNGImage();
    ImageConversionError load();
    size_t convertToR5G6B5(uint16_t* outputArray);
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    const char* filename;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep* row_pointers;
    int width, height;

    uint16_t convertRGBToR5G6B5(uint8_t r, uint8_t g, uint8_t b);
};

// API function to convert PNG to R5G6B5
ImageConversionError
convertPNGToR5G6B5(const char* inputFilePath, uint16_t* outputArray, size_t* outputLength, int* width, int* height);

#endif
