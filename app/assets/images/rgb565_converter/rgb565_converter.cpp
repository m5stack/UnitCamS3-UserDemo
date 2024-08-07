/**
 * @file rgb565_converter.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-07-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifdef ENABLE_PNG_CONVERTOR
#include "rgb565_converter.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h> // for htons

PNGImage::PNGImage(const char* filename) : filename(filename), png_ptr(nullptr), info_ptr(nullptr), row_pointers(nullptr) {}

PNGImage::~PNGImage()
{
    if (row_pointers)
    {
        for (int y = 0; y < height; y++)
        {
            free(row_pointers[y]);
        }
        free(row_pointers);
    }
    if (info_ptr)
        png_destroy_info_struct(png_ptr, &info_ptr);
    if (png_ptr)
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
}

ImageConversionError PNGImage::load()
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        return ImageConversionError::FileNotFound;
    }

    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
        fclose(fp);
        return ImageConversionError::NotAPNG;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr)
    {
        fclose(fp);
        return ImageConversionError::InitError;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        fclose(fp);
        return ImageConversionError::InitError;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return ImageConversionError::ReadError;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    if (!row_pointers)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return ImageConversionError::MemoryError;
    }

    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
        if (!row_pointers[y])
        {
            for (int i = 0; i < y; ++i)
            {
                free(row_pointers[i]);
            }
            free(row_pointers);
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
            fclose(fp);
            return ImageConversionError::MemoryError;
        }
    }

    png_read_image(png_ptr, row_pointers);
    fclose(fp);
    return ImageConversionError::Success;
}

size_t PNGImage::convertToR5G6B5(uint16_t* outputArray)
{
    size_t pixelCount = width * height;

    if (outputArray == nullptr)
        return 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            png_bytep px = &(row_pointers[y][x * 4]);
            uint16_t r5g6b5 = convertRGBToR5G6B5(px[0], px[1], px[2]);
            outputArray[y * width + x] = htons(r5g6b5); // Convert to big-endian
        }
    }
    return pixelCount; // Return the number of pixels
}

uint16_t PNGImage::convertRGBToR5G6B5(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t r5 = (r >> 3) & 0x1F;
    uint16_t g6 = (g >> 2) & 0x3F;
    uint16_t b5 = (b >> 3) & 0x1F;
    return (r5 << 11) | (g6 << 5) | b5;
}

ImageConversionError
convertPNGToR5G6B5(const char* inputFilePath, uint16_t* outputArray, size_t* outputLength, int* width, int* height)
{
    PNGImage pngImage(inputFilePath);
    ImageConversionError loadResult = pngImage.load();

    if (loadResult != ImageConversionError::Success)
    {
        return loadResult; // Return error if loading fails
    }

    size_t pixelCount = pngImage.convertToR5G6B5(outputArray);
    if (pixelCount == 0)
    {
        return ImageConversionError::ConversionError; // Return error if conversion fails
    }

    *outputLength = pixelCount;           // Set output length
    *width = pngImage.getWidth();         // Set output width
    *height = pngImage.getHeight();       // Set output height
    return ImageConversionError::Success; // Return success
}

#endif
