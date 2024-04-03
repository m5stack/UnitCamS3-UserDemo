/**
 * @file api_mic.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include "Arduino.h"
#include "api_mic.h"
#include "../../hal/hal.h"
#include <LittleFS.h>



static bool _is_mic_recording = false;
static SemaphoreHandle_t _xSemaphore_is_recording = NULL;


static constexpr const size_t record_length = 256;
static constexpr const size_t record_number = 4;
static constexpr const size_t record_size = record_number * record_length;
static constexpr const size_t record_samplerate = 16000;
static size_t record_index = 0;
static int16_t record_data[record_size];

static int16_t *rec_psram_data;
static size_t rec_psram_index = 0;
// static constexpr const size_t rec_psram_maxlen = 1024*1024;
// static constexpr const size_t rec_psram_maxlen = 1024 * 512;

// 8s
// static constexpr const size_t rec_psram_maxlen = 1024 * 256;

// 4s
static constexpr const size_t rec_psram_maxlen = 1024 * 127;

static constexpr const size_t maxfilesize = rec_psram_maxlen * 2 + 44;

struct __attribute__((packed)) wav_header_t
{
  char RIFF[4] = { 0x52, 0x49, 0x46, 0x46 };
  uint32_t chunk_size = maxfilesize - 8;
  char WAVEfmt[8] = { 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20 };
  uint32_t fmt_chunk_size = 0x10;
  uint16_t audiofmt = 0x01;
  uint16_t channel = 0x02;
  uint32_t sample_rate = record_samplerate;
  uint32_t byte_per_sec = record_samplerate * 4;
  uint16_t block_size = 4;
  uint16_t bit_per_sample = 16;
  char identifier[4] = { 0x64,0x61,0x74,0x61 };
  uint32_t sub_chunk_size = maxfilesize - 44;
};

static void _mic_recording_task(void* param)
{
    xSemaphoreTake(_xSemaphore_is_recording, portMAX_DELAY);
    _is_mic_recording = true;
    xSemaphoreGive(_xSemaphore_is_recording);

    spdlog::info("start mic recording...");

    {
        auto mic_cfg = HAL::hal::GetHal()->mic->config();
        mic_cfg.dma_buf_count = 8;
        mic_cfg.dma_buf_len = 512;
        mic_cfg.stereo = true;
        HAL::hal::GetHal()->mic->config(mic_cfg);
    }

    /* Alloc buffer from psram */
    rec_psram_data = nullptr;
    rec_psram_data = (typeof(rec_psram_data))heap_caps_malloc(rec_psram_maxlen * sizeof(int16_t), MALLOC_CAP_SPIRAM);
    if (rec_psram_data == nullptr)
    {
        printf("[MicRecTest] buffer alloc failed\n");
        return;
    }
    printf("[MicRecTest] buffer alloc ok\n");

    memset(rec_psram_data, 0 , rec_psram_maxlen * sizeof(int16_t));
    rec_psram_index = 0;
    record_index = 0;


    fs::File outfile;
    static size_t file_number = -1;
    size_t write_size = 0;
    char filename[32];

    // snprintf(filename, sizeof(filename), "/wav/rec_%04d.wav", ++file_number);
    snprintf(filename, sizeof(filename), "/wav/rec.wav");
    

    printf("[MicRecTest] start recording...\n");

    size_t bytes_read = 0;
    while (1)
    {
        record_index = (++record_index) & (record_number - 1);
        auto rec_buf  = &record_data[record_index  * record_length];
        auto draw_record_idx = (record_index - 2) & (record_number - 1);
        auto draw_buf = &record_data[draw_record_idx * record_length];



        HAL::hal::GetHal()->mic->record(rec_buf, record_length, record_samplerate);
        while (HAL::hal::GetHal()->mic->isRecording());


        memcpy(&rec_psram_data[rec_psram_index], draw_buf, record_length * sizeof(int16_t));
        rec_psram_index += record_length;

        if (rec_psram_index >= rec_psram_maxlen)
        {
            printf("[MicRecTest] finish recording\n");

            // outfile = SD.open(filename, "w", true);
            outfile = LittleFS.open(filename, "w", true);

            wav_header_t header;
            header.chunk_size = rec_psram_index * sizeof(int16_t) + 36;
            header.sub_chunk_size = rec_psram_index * sizeof(int16_t);
            
            if (outfile)
            {
                outfile.write((const uint8_t*)&header, sizeof(wav_header_t));
                outfile.write((const uint8_t*)rec_psram_data, rec_psram_index * sizeof(int16_t));
                outfile.close();

                printf("[MicRecTest] file saved in %s\n", filename);
            }
            else 
            {
                printf("[MicRecTest] file open failed\n");
            }

            break;
        }
    }

    free(rec_psram_data);


    xSemaphoreTake(_xSemaphore_is_recording, portMAX_DELAY);
    _is_mic_recording = false;
    xSemaphoreGive(_xSemaphore_is_recording);

    spdlog::info("mic recording done");

    vTaskDelete(NULL);
}




void startMicRecording(AsyncWebServerRequest* request)
{
    bool is_recording = true;

    /* See if we can obtain the semaphore.  If the semaphore is not
    available wait 10 ticks to see if it becomes free. */
    if( xSemaphoreTake(_xSemaphore_is_recording, pdMS_TO_TICKS(2000)) == pdTRUE)
    {
        is_recording = _is_mic_recording;
        xSemaphoreGive(_xSemaphore_is_recording);
    }
    else
    {
        is_recording = true;
    }

    if (!is_recording)
        xTaskCreatePinnedToCore(_mic_recording_task, "mic", 4000, NULL, 3, NULL, 1);

    // xTaskCreate(_mic_recording_task, "mic", 4000, NULL, 15, NULL);
    request->send(200, "application/json", "{\"msg\":\"ok\"}");
}


void isMicRecording(AsyncWebServerRequest* request)
{
    String msg;
    bool is_recording = true;

    /* See if we can obtain the semaphore.  If the semaphore is not
    available wait 10 ticks to see if it becomes free. */
    if( xSemaphoreTake(_xSemaphore_is_recording, pdMS_TO_TICKS(2000)) == pdTRUE)
    {
        is_recording = _is_mic_recording;
        xSemaphoreGive(_xSemaphore_is_recording);
    }
    else
    {
        is_recording = true;
    }


    if (is_recording)
        msg = "{\"msg\":\"yes\"}";
    else 
        msg = "{\"msg\":\"no\"}";

    request->send(200, "application/json", msg);
}


void load_mic_apis(AsyncWebServer& server)
{
    /* Create a mutex type semaphore. */
    _xSemaphore_is_recording = xSemaphoreCreateMutex();

    server.on("/api/v1/mic_start", HTTP_GET, startMicRecording);
    server.on("/api/v1/mic_is_recording", HTTP_GET, isMicRecording);
}
