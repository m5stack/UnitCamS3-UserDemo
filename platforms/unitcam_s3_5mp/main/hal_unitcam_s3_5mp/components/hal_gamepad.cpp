/**
 * @file hal_gamepad.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-06-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_unitcam_s3_5mp.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <driver/gpio.h>

void HAL_UnitCamS3_5MP::_gamepad_init()
{
    spdlog::info("gamepad init");

    gpio_reset_pin((gpio_num_t)HAL_PIN_BUTTON_A);
    gpio_set_direction((gpio_num_t)HAL_PIN_BUTTON_A, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_BUTTON_A, GPIO_FLOATING);

    gpio_reset_pin((gpio_num_t)HAL_PIN_LED);
    gpio_set_direction((gpio_num_t)HAL_PIN_LED, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_LED, GPIO_PULLUP_ONLY);

    // /* -------------------------------------------------------------------------- */
    // /*                                    Test                                    */
    // /* -------------------------------------------------------------------------- */
    // while (1)
    // {
    //     SetLedState(true);
    //     delay(500);
    //     SetLedState(false);
    //     delay(400);
    //     feedTheDog();
    // }
}

bool HAL_UnitCamS3_5MP::getButton(GAMEPAD::GamePadButton_t button)
{
    if (button == GAMEPAD::BTN_A)
        return !gpio_get_level((gpio_num_t)HAL_PIN_BUTTON_A);
    return false;
}

void HAL_UnitCamS3_5MP::setLedState(bool state)
{
    // spdlog::info("set led: {}", state);
    gpio_set_level((gpio_num_t)HAL_PIN_LED, state ? 0 : 1);
}
