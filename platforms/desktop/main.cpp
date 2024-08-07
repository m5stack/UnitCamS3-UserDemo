/**
 * @file main.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <app.h>
#include "hal_desktop/hal_desktop.hpp"

void setup()
{
    APP::SetupCallback_t callback;

    callback.AssetPoolInjection = []() {
        // AssetPool::InjectStaticAsset(AssetPool::CreateStaticAsset());

        // Create and output to bin
        AssetPool::CreateStaticAssetBin(AssetPool::CreateStaticAsset());
        // Load bin and inject
        AssetPool::InjectStaticAsset(AssetPool::GetStaticAssetFromBin());
    };

    callback.HalInjection = []() {
        // HAL::Inject(new HAL_Desktop(320, 240));
        HAL::Inject(new HAL_Desktop(128, 128));
        // HAL::Inject(new HAL_Desktop(128, 128, true));
    };

    APP::Setup(callback);
}

void loop() { APP::Loop(); }
