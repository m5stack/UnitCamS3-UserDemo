/**
 * @file mooncake.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.2
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

/* Disable some features */
#ifdef ESP_PLATFORM
#define SPDLOG_NO_EXCEPTIONS
#define SPDLOG_NO_THREAD_ID
#endif
#include "spdlog/include/spdlog/spdlog.h"
