/**
 * @file hal_unitcam_s3_5mp.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-08-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <hal/hal.h>
#include <Arduino.h>

class HAL_UnitCamS3_5MP : public HAL
{
private:
    void _watch_dog_init();
    void _gamepad_init();
    void _camera_init();
    void _fs_init();
    void _log_out_system_config();
    void _mic_init();

public:
    std::string type() override { return "UnitCamS3-5MP"; }

    inline void init() override
    {
        _gamepad_init();
        _watch_dog_init();
        _camera_init();

        initArduino();
        _fs_init();
        _mic_init();

        // while (1)
        // {
        //     getSdCardInfo();
        //     feedTheDog();
        //     delay(2000);
        // }
    }

    /* -------------------------------------------------------------------------- */
    /*                             Public api override                            */
    /* -------------------------------------------------------------------------- */
    void feedTheDog() override;
    bool getButton(GAMEPAD::GamePadButton_t button) override;
    void loadSystemConfig() override;
    void saveSystemConfig() override;
    std::string getSdCardInfo() override;
    void setLedState(bool state) override;

    void startApServer() override;
    void stopApServer() override;
    void startPosterServer() override;

    bool sdCardInit(bool passImagePath) override;
    bool sdCardDeInit() override;
    bool isSdCardVaild() override;
    bool saveImage(uint8_t* img, size_t size) override;
};
