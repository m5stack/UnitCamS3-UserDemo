// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sccb.h"
#include "mega_ccm.h"
#include "mega_ccm_regs.h"
#include "mega_ccm_settings.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#else
#include "esp_log.h"
static const char *TAG = "mega_ccm";
#endif

#define H8(v) ((v)>>8)
#define L8(v) ((v)&0xff)
#define brightness_max   8
#define contarst_max     6
#define saturation_max   6
#define exposure_max     6
#define awb_mode_max     4
#define special_max      6
#define quality_max      2
#define mirror_max       2
#define flip_max         2
#define agc_mode_max     1
#define analog_gain_max  6
#define mamual_exp_h_max 6
#define mamual_exp_l_max 6
//#define REG_DEBUG_ON

static int read_reg(uint8_t slv_addr, const uint16_t reg)
{
    int ret = SCCB_Read16(slv_addr, reg);
    return ret;
}

static int write_reg(uint8_t slv_addr, const uint16_t reg, uint8_t value)
{
    int ret = 0;

  ret = SCCB_Write16(slv_addr,reg, value);
    return ret;
}


static void print_regs(uint8_t slv_addr)
{
#ifdef DEBUG_PRINT_REG
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, "REG list look ======================");
    for (size_t i = 0xf0; i <= 0xfe; i++) {
        ESP_LOGI(TAG, "reg[0x%02x] = 0x%02x", i, read_reg(slv_addr, i));
    }
    ESP_LOGI(TAG, "\npage 0 ===");
    write_reg(slv_addr, 0xfe, 0x00); // page 0
    for (size_t i = 0x03; i <= 0x24; i++) {
        ESP_LOGI(TAG, "p0 reg[0x%02x] = 0x%02x", i, read_reg(slv_addr, i));
    }
    for (size_t i = 0x40; i <= 0x95; i++) {
        ESP_LOGI(TAG, "p0 reg[0x%02x] = 0x%02x", i, read_reg(slv_addr, i));
    }
    ESP_LOGI(TAG, "\npage 3 ===");
    write_reg(slv_addr, 0xfe, 0x03); // page 3
    for (size_t i = 0x01; i <= 0x43; i++) {
        ESP_LOGI(TAG, "p3 reg[0x%02x] = 0x%02x", i, read_reg(slv_addr, i));
    }
#endif
}


static int reset(sensor_t *sensor)
{
    int ret = 0;
    // Reset mega-ccm: clear all registers and reset them to their default values
    ret = write_reg(sensor->slv_addr, CAMERA_RST_REG, 0x00);
    if (ret) {
        ESP_LOGE(TAG, "Software Reset FAILED!");
        return ret;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
     ESP_LOGI(TAG, "Software Reset  slv_addr %x address: %x, data: %x",sensor->slv_addr,CAMERA_RST_REG,0x01);

    ret = write_reg(sensor->slv_addr, CAMERA_RST_REG, 0x01);
    if (ret) {
        ESP_LOGE(TAG, "Software Reset FAILED!");
        return ret;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // add 1s delay to wait mega-ccm init normal
    



    return ret;
}

// static int set_Camera_rest(sensor_t *sensor, int level)
// {
//     int ret = 0;
//     if (level > camera_rest_max) {
//         ESP_LOGW(TAG, "Invalid Camera_rest: %u", level);
//         level = camera_rest_max;
//     }
//     ret = write_reg(sensor->slv_addr, CAMERA_RST_REG, level);
//     if (ret == 0) {
//         ESP_LOGD(TAG, "Set Camera_rest to: %d", level);
//     }
//     return ret;
// }
static int set_pixformat(sensor_t *sensor, pixformat_t pixformat)
{
    int ret = 0;
    switch (pixformat) {
    case PIXFORMAT_JPEG:
        ret = write_reg(sensor->slv_addr, PIXEL_FMT_REG, 0x01); //JPEG
        break;

    case PIXFORMAT_RGB565:
        ret = write_reg(sensor->slv_addr, PIXEL_FMT_REG, 0x10); //RGB565
        break;

    case PIXFORMAT_YUV422:
        ret = write_reg(sensor->slv_addr, PIXEL_FMT_REG, 0x11); //YUV422
        break;
    
    default:
        ESP_LOGW(TAG, "unsupport format");
        ret = -1;
        break;
    }
    if (ret == 0) {
        sensor->pixformat = pixformat;
        ESP_LOGD(TAG, "Set pixformat to: %u", pixformat);
    }

    return ret;
}
static int set_framesize(sensor_t *sensor, framesize_t framesize)
{
    int ret = 0;
    if (framesize > FRAMESIZE_5MP) {
        ESP_LOGW(TAG, "Invalid framesize: %u", framesize);
        framesize = FRAMESIZE_5MP;
    }
    sensor->status.framesize = framesize;
    uint16_t w = resolution[framesize].width;
    uint16_t h = resolution[framesize].height;
    
   if(framesize == FRAMESIZE_QVGA){   //320x240
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x01);
    }else if(framesize == FRAMESIZE_VGA){   //640x480
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x02);
    }else if(framesize == FRAMESIZE_HD){   //1280x720
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x03);
    }else if(framesize == FRAMESIZE_UXGA){   //1600x1200
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x04);
    }else if(framesize == FRAMESIZE_FHD){   //1920x1080
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x05);
    }else if(framesize == FRAMESIZE_5MP){   //2592x1944
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x06);
    }else if (framesize == FRAMESIZE_96X96){  //96x96
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x07);
    }else if(framesize == FRAMESIZE_128x128){  //128x128
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x08);
    }else if(framesize == FRAMESIZE_320x320){  //320x320
        ret = write_reg(sensor->slv_addr, RESOLUTION_REG, 0x09);
    }
    if (ret == 0) {
        ESP_LOGI(TAG, "Set framesize to: %ux%u", w, h);
    }
    print_regs(sensor->slv_addr);
    return ret;
}

static int set_brightness(sensor_t *sensor, int level)
{
    int ret = 0;
    if (level > brightness_max) {
        ESP_LOGW(TAG, "Invalid brightness: %u", level);
        level = brightness_max;
    }
    switch (level) {
    case brightness_0:
       ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x08); //-4
        break;  
    case brightness_1:
        ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x06); //-3
        break;
    case brightness_2:
        ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x04); //-2
        break;
    case brightness_3:
        ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x02); //-1  
        break;
    case brightness_4:
         ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x00); //default
        break;
    case brightness_5:
       ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x01); //+1
        break;  
    case brightness_6:
        ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x03); //+2
        break;
    case brightness_7:
        ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x05); //+3  
        break;
    case brightness_8:
    ret = write_reg(sensor->slv_addr, BRIGHTNESS_REG, 0x07); //+4
      break;  
    default:
        ESP_LOGW(TAG, "brightness fail");
        ret = -1;
        break;
    }
    if (ret == 0) {
        ESP_LOGD(TAG, "Set brightness to: %d", level);
    }
    return ret;
}

static int set_contrast(sensor_t *sensor, int level)
{
    int ret = 0;
    if (level > contarst_max) {
        ESP_LOGW(TAG, "Invalid contrast: %u", level);
        level = contarst_max;
    }
    switch (level) {
    case contrast_0:
        ret = write_reg(sensor->slv_addr, CONTRAST_REG, 0x06); //-3
        break;
    case contrast_1:
        ret = write_reg(sensor->slv_addr, CONTRAST_REG, 0x04); //-2
        break;
    case contrast_2:
        ret = write_reg(sensor->slv_addr, CONTRAST_REG, 0x02); //-1
        break;
    case contrast_3:
        ret = write_reg(sensor->slv_addr, CONTRAST_REG, 0x00); //default
        break;
    case contrast_4:
        ret = write_reg(sensor->slv_addr, CONTRAST_REG, 0x01); //+1
        break;
    case contrast_5:
        ret = write_reg(sensor->slv_addr, CONTRAST_REG, 0x03); //+2 
        break;  
    case contrast_6:
        ret = write_reg(sensor->slv_addr, CONTRAST_REG, 0x05); //+3
        break;  
    default:
        ESP_LOGW(TAG, "contrast fail");
        ret = -1;
        break;
    }
    if (ret == 0) {
        ESP_LOGD(TAG, "Set contrast to: %d", level);
    }
    return ret;
}

static int set_saturation(sensor_t *sensor, int level)
{
    int ret = 0;
    if (level > saturation_max) {
        ESP_LOGW(TAG, "Invalid saturation: %u", level);
        level = saturation_max;
    }
    switch (level) {
    case saturation_0:
        ret = write_reg(sensor->slv_addr, SATURATION_REG, 0x06); //-3
        
        break;
    case saturation_1:
        ret = write_reg(sensor->slv_addr, SATURATION_REG, 0x04); //-2
        
        break;

    case saturation_2:
        ret = write_reg(sensor->slv_addr, SATURATION_REG, 0x02); //-1
        break;
    case saturation_3:
        ret = write_reg(sensor->slv_addr, SATURATION_REG, 0x00); //default
        
        break;
    case saturation_4:
        ret = write_reg(sensor->slv_addr, SATURATION_REG, 0x01); //+1
        break;
    case saturation_5:
        ret = write_reg(sensor->slv_addr, SATURATION_REG, 0x03); //+2
        
        break;  
    case saturation_6:
        ret = write_reg(sensor->slv_addr, SATURATION_REG, 0x05); //+3
        break;  
    default:
        ESP_LOGW(TAG, "saturation fail");
        ret = -1;
        break;
    }
    if (ret == 0) {
        ESP_LOGD(TAG, "Set saturation to: %d", level);
    }
    return ret;
}

static int set_exposure_ctrl(sensor_t *sensor, int enable)
{
    int ret = 0;
    if (enable > exposure_max) {
        ESP_LOGW(TAG, "Invalid exposure: %u", enable);
        enable = exposure_max;
    }
    switch (enable) {
    case exposure_0:
        ret = write_reg(sensor->slv_addr, EXP_COMPENSATE_REG, 0x00); //default
        break;
    case exposure_1:
        ret = write_reg(sensor->slv_addr, EXP_COMPENSATE_REG, 0x01); //+1
        break;
    case exposure_2:
        ret = write_reg(sensor->slv_addr, EXP_COMPENSATE_REG, 0x02); //-1
        break;
    case exposure_3:
        ret = write_reg(sensor->slv_addr, EXP_COMPENSATE_REG, 0x03); //+2
        break;
    case exposure_4:
        ret = write_reg(sensor->slv_addr, EXP_COMPENSATE_REG, 0x04); //-2
        break;
    case exposure_5:
        ret = write_reg(sensor->slv_addr, EXP_COMPENSATE_REG, 0x05); //+3
        break;  
    case exposure_6:
        ret = write_reg(sensor->slv_addr, EXP_COMPENSATE_REG, 0x06); //-3
        break;  
    default:
        ESP_LOGW(TAG, "exposure fail");
        ret = -1;
        break;
    }
    if (ret == 0) {
        ESP_LOGD(TAG, "Set exposure to: %d", enable);
    }
    return ret;
}
static int set_wb_mode (sensor_t *sensor, int mode)
{
    int ret = 0;
    if (mode > awb_mode_max) {
        ESP_LOGW(TAG, "Invalid AWB_mode : %u", mode);
        mode = awb_mode_max;
    }
    switch (mode) {
    case Auto:
        ret = write_reg(sensor->slv_addr, AWB_MODE_REG, 0x00); 
        break;
    case sunny:
        ret = write_reg(sensor->slv_addr, AWB_MODE_REG, 0x01); 
        break;
    case office:
        ret = write_reg(sensor->slv_addr, AWB_MODE_REG, 0x02);
        break;
    case cloudy:
        ret = write_reg(sensor->slv_addr, AWB_MODE_REG, 0x03);
        break;
    case home:
        ret = write_reg(sensor->slv_addr, AWB_MODE_REG, 0x04); 
        break; 
    default:
        ESP_LOGW(TAG, "AWB_mode fail");
        ret = -1;
        break;
    }
    if (ret == 0) {
        ESP_LOGD(TAG, "Set AWB_mode to: %d", mode);
    }
    return ret;
}

static int set_special_effect (sensor_t *sensor, int effect)
{
    int ret = 0;
     if (effect > special_max) {
        ESP_LOGW(TAG, "Invalid special effect: %u", effect);
        effect = special_max;
    }
    switch (effect) {
    case normal:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x00); 
        break;
    case blueish:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x01); 
        break;
    case redish:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x02); 
        break;
    case BorW:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x03); 
        break;
    case sepia:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x04); 
        break;
    case negative:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x05); 
        break;
    case greenish:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x06); 
        break;
    default:
     ret = write_reg(sensor->slv_addr, SPECIAL_REG, 0x00); 
        break;
    }
    if (ret == 0) {
        ESP_LOGD(TAG, "Set special effect to: %d", effect);
    }
    return ret;
}
static int set_quality (sensor_t *sensor, int quality)
{
    int ret = 0;
   if (quality > quality_max) {
        ESP_LOGW(TAG, "Invalid quality  : %u", quality);
        quality = quality_max;
    }
     switch (quality) {
     case quality_high:
        ret = write_reg(sensor->slv_addr, IMAGE_QUALITY_REG, 0x00); 
        break;
        case quality_default:
             ret = write_reg(sensor->slv_addr, IMAGE_QUALITY_REG, 0x01); 
        break;
        case quality_low:
             ret = write_reg(sensor->slv_addr, IMAGE_QUALITY_REG, 0x02); 
        break;
     }
    return ret;
}


static int set_mirror (sensor_t *sensor, int mirror)
{
    int ret = 0;
   if (mirror > mirror_max) {
        ESP_LOGW(TAG, "Invalid mirror  : %u", mirror);
        mirror = mirror_max;
    }
     switch (mirror) {
     case mirror_enable:
        ret = write_reg(sensor->slv_addr, IMAGE_MIRROR_REG, 0x01); 
        break;
        case mirror_disable:
             ret = write_reg(sensor->slv_addr, IMAGE_MIRROR_REG, 0x00); 
        break;
     }
    return ret;
}
static int set_flip (sensor_t *sensor, int flip)
{
    int ret = 0;
   if (flip > flip_max) {
        ESP_LOGW(TAG, "Invalid flip  : %u", flip);
        flip = mirror_max;
    }
     switch (flip) {
     case flip_enable:
        ret = write_reg(sensor->slv_addr, IMAGE_FLIP_REG, 0x01); 
        break;
        case flip_disable:
             ret = write_reg(sensor->slv_addr, IMAGE_FLIP_REG, 0x00); 
        break;
     }
    return ret;
}


static int set_AGC_mode  (sensor_t *sensor, int mode)
{
    int ret = 0;
    if (mode > agc_mode_max) {
        ESP_LOGW(TAG, "Invalid AGC_mode  : %u", mode);
        mode = agc_mode_max;
    }
    switch (mode) {
    case AGC_Auto:
        ret = write_reg(sensor->slv_addr, AGC_MODE_REG, 0x00); 
        break;
    case AGC_Manual:
        ret = write_reg(sensor->slv_addr, AGC_MODE_REG, 0x01); 
        break;
    default:
        ESP_LOGW(TAG, "AGC_mode fail");
        ret = -1;
        break;
    }
    if (ret == 0) {
        ESP_LOGD(TAG, "Set AGC_mode  to: %d", mode);
    }
    return ret;
}

static int set_agc_gain   (sensor_t *sensor, int gain)
{
    int ret = 0;
    if (gain > analog_gain_max) {
        ESP_LOGW(TAG, "Invalid analog_gain   : %u", gain);
        gain = analog_gain_max;
    }
    ret = write_reg(sensor->slv_addr, MANUAL_AGC_REG, gain);
    if (ret == 0) {
        ESP_LOGD(TAG, "Set analog_gain  to: %d", gain);
    }
    return ret;
}

static int set_mamual_exp_h   (sensor_t *sensor, int level)
{
    int ret = 0;
    if (level > mamual_exp_h_max) {
        ESP_LOGW(TAG, "Invalid mamual_exp_h    : %u", level);
        level = mamual_exp_h_max;
    }
    ret = write_reg(sensor->slv_addr, MANUAL_EXP_H_REG , level);
    if (ret == 0) {
        ESP_LOGD(TAG, "Set mamual_exp_h   to: %d", level);
    }
    return ret;
}

static int set_mamual_exp_l   (sensor_t *sensor, int level)
{
    int ret = 0;
    if (level > mamual_exp_l_max) {
        ESP_LOGW(TAG, "Invalid mamual_exp_l    : %u", level);
        level = mamual_exp_l_max;
    }
    ret = write_reg(sensor->slv_addr, MANUAL_EXP_L_REG , level);
    if (ret == 0) {
        ESP_LOGD(TAG, "Set mamual_exp_l   to: %d", level);
    }
    return ret;
}


static int get_reg(sensor_t *sensor, int reg, int mask)
{
    int ret = 0;
    if (mask > 0xFF) {
        ESP_LOGE(TAG, "mask should not more than 0xff");
    } else {
        ret = read_reg(sensor->slv_addr, reg);
    }
    if (ret > 0) {
        ret &= mask;
    }
    return ret;
}

static int set_reg(sensor_t *sensor, int reg, int mask, int value)
{
    int ret = 0;
    if (mask > 0xFF) {
        ESP_LOGE(TAG, "mask should not more than 0xff");
    } else {
        ret = read_reg(sensor->slv_addr, reg);
    }
    if (ret < 0) {
        return ret;
    }
    value = (ret & ~mask) | (value & mask);

    if (mask > 0xFF) {

    } else {
        ret = write_reg(sensor->slv_addr, reg, value);
    }
    return ret;
}

static int init_status(sensor_t *sensor)
{
     ESP_LOGW(TAG, "Unsupported");
    return 0;
}

static int set_dummy(sensor_t *sensor, int val)
{
    ESP_LOGW(TAG, "Unsupported");
    return -1;
}
static int set_gainceiling_dummy(sensor_t *sensor, gainceiling_t val)
{
    ESP_LOGW(TAG, "Unsupported");
    return -1;
}

int mega_ccm_detect(int slv_addr, sensor_id_t *id)
{
    if (MEGA_CCM_SCCB_ADDR == slv_addr) {
        uint8_t MIDH = SCCB_Read(slv_addr, UW_DEVICE_ID_HI);
        uint8_t MIDL = SCCB_Read(slv_addr, UW_DEVICE_ID_LO);
        uint16_t PID = MIDH << 8 | MIDL;
        if (MEGA_CCM_PID == PID) {
            id->PID = PID;
            return PID;
        } else {
            ESP_LOGI(TAG, "Mismatch PID=0x%x", PID);
        }
    }
    return 0;
}

int mega_ccm_init(sensor_t *sensor)
{
    sensor->init_status = init_status;
    sensor->reset = reset;
    sensor->set_pixformat = set_pixformat;
    sensor->set_framesize = set_framesize;
    sensor->set_brightness = set_brightness;
    sensor->set_contrast = set_contrast;
    sensor->set_saturation = set_saturation;
    sensor->set_exposure_ctrl = set_exposure_ctrl;
    sensor->set_wb_mode = set_wb_mode;
    sensor->set_special_effect = set_special_effect;
    sensor->set_quality = set_quality;
    sensor->set_AGC_mode = set_AGC_mode;
    sensor->set_agc_gain = set_agc_gain;     
    sensor->set_mamual_exp_h = set_mamual_exp_h;
    sensor->set_mamual_exp_l= set_mamual_exp_l;
    sensor->set_gainceiling = set_gainceiling_dummy;

    sensor->set_sharpness = set_dummy;
    sensor->set_denoise = set_dummy;
    sensor->set_colorbar = set_dummy;
    sensor->set_whitebal = set_dummy;
    sensor->set_gain_ctrl = set_dummy;
    sensor->set_hmirror = set_mirror;
    sensor->set_vflip = set_flip;
    sensor->set_aec2 = set_dummy;
    sensor->set_awb_gain = set_dummy;
    sensor->set_aec_value = set_dummy;
    sensor->set_ae_level = set_dummy;
    sensor->set_dcw = set_dummy;
    sensor->set_bpc = set_dummy;
    sensor->set_wpc = set_dummy;
    sensor->set_raw_gma = set_dummy;
    sensor->set_lenc = set_dummy;
    sensor->get_reg = get_reg;
    sensor->set_reg = set_reg;
    sensor->set_res_raw = NULL;
    sensor->set_pll = NULL;
    sensor->set_xclk = NULL;

    ESP_LOGD(TAG, "mega_ccm Attached");
    return 0;
}
