/*
 * GC032A register definitions.
 */
#ifndef __MEGA_CCM_REG_REGS_H__
#define __MEGA_CCM_REG_REGS_H__




#endif //__MEGA_CCM_REG_REGS_H__

#define ID_BASE                 					    0x0000
#define SENSOR_BASE                 					0x0100

#define UW_DEVICE_ID_HI             ID_BASE | 0x00
#define UW_DEVICE_ID_LO             ID_BASE | 0x01
#define FIRMWARE_VER                ID_BASE | 0x02

#define CAMERA_RST_REG              SENSOR_BASE|0x02


#define PIXEL_FMT_REG               SENSOR_BASE|0x20
#define RESOLUTION_REG              SENSOR_BASE|0x21
#define BRIGHTNESS_REG              SENSOR_BASE|0x22
#define CONTRAST_REG              	SENSOR_BASE|0x23
#define SATURATION_REG              SENSOR_BASE|0x24
#define EXP_COMPENSATE_REG          SENSOR_BASE|0x25
#define AWB_MODE_REG                SENSOR_BASE|0x26
#define SPECIAL_REG                 SENSOR_BASE|0x27
#define SHARPNESS_REG               SENSOR_BASE|0x28
#define FOCUS_REG                   SENSOR_BASE|0x29
#define IMAGE_QUALITY_REG           SENSOR_BASE|0x2A
#define IMAGE_FLIP_REG           	SENSOR_BASE|0x2B
#define IMAGE_MIRROR_REG            SENSOR_BASE|0x2C



#define AGC_MODE_REG				SENSOR_BASE|0x30
#define MANUAL_AGC_REG          	SENSOR_BASE|0x31
#define AE_MODE_REG					SENSOR_BASE|0x32
#define MANUAL_EXP_H_REG            SENSOR_BASE|0x33
#define MANUAL_EXP_L_REG            SENSOR_BASE|0x34
#define BYPASS                      0xFFF0

#define CAM_RST_BIT                  ( 1 << 0 )
#define CAM_PWDN_BIT                 ( 1 << 1 )
#define CAM_PWEN_BIT                 ( 1 << 2 )