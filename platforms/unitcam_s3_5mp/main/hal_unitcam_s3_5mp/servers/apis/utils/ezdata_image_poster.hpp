/**
 * @file ezdata_image_poster.hpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-12-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <Arduino.h>
#include <esp_camera.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <functional>

static const char* _server_host = "ezdata2.m5stack.com";
static const uint16_t _server_port = 80;

struct UploadData_t
{
    std::string string_buffer;
    std::string form_data_part_1;
    std::string form_data_part_2;
};

static bool ezdata_image_poster(std::string mac,
                                std::string nickname,
                                std::string timeZone,
                                std::function<void(camera_fb_t* frameBuffer)> onCaptured = nullptr)
{
    /* -------------------------------------------------------------------------- */
    /*                                   Capture                                  */
    /* -------------------------------------------------------------------------- */
    camera_fb_t* fb = NULL;
    fb = esp_camera_fb_get();
    if (!fb)
    {
        spdlog::error("capture failed");
        return false;
    }
    spdlog::info("capture ok, size: {}x{} in {} bytes", fb->width, fb->height, fb->len);
    if (onCaptured != nullptr)
        onCaptured(fb);

    /* -------------------------------------------------------------------------- */
    /*                                   Upload                                   */
    /* -------------------------------------------------------------------------- */
    UploadData_t data;

    /* -------------------------------------------------------------------------- */
    /*                                  Form data                                 */
    /* -------------------------------------------------------------------------- */
    data.form_data_part_1 = spdlog::fmt_lib::format(
        "\r\nContent-Type: multipart/form-data; "
        "boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: "
        "form-data; name=\"dataType\"\r\n\r\nfile\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: "
        "form-data; name=\"watermarkText\"\r\n\r\n{}\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: "
        "form-data; name=\"timeZoneId\"\r\n\r\n{}\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: "
        "form-data; name=\"name\"\r\n\r\ncaptured\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: "
        "form-data; name=\"permissions\"\r\n\r\n1\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: "
        "form-data; name=\"value\"\r\n\r\ncaptured.jpg\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: "
        "form-data; name=\"file\"; filename=\"captured.jpg\"\r\nContent-Type: image/jpeg\r\n",
        nickname,
        timeZone);

    data.form_data_part_2 = spdlog::fmt_lib::format("\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n");

    /* -------------------------------------------------------------------------- */
    /*                                   Request                                  */
    /* -------------------------------------------------------------------------- */
    spdlog::info("connect to server {}:{}", _server_host, _server_port);

    // Connect server
    WiFiClient client;
    if (!client.connect(_server_host, _server_port))
    {
        spdlog::error("connect to server failed");
        esp_camera_fb_return(fb);
        return false;
    }

    int content_length = data.form_data_part_1.size() + fb->len + data.form_data_part_2.size();
    spdlog::info("get content length {}", content_length);

    /* --------------------------------- Header --------------------------------- */
    spdlog::info("send header");

    data.string_buffer = spdlog::fmt_lib::format("POST /api/v2/{}/uploadMacFile HTTP/1.1", mac);
    client.println(data.string_buffer.c_str());
    // printf("%s\r\n", data.string_buffer.c_str());

    data.string_buffer = spdlog::fmt_lib::format("Host: {}:{}", _server_host, _server_port);
    client.println(data.string_buffer.c_str());
    // printf("%s\r\n", data.string_buffer.c_str());

    data.string_buffer = spdlog::fmt_lib::format("Content-Length: {}", content_length);
    client.println(data.string_buffer.c_str());
    // printf("%s\r\n", data.string_buffer.c_str());

    client.println("Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
    // printf("Content-Type: multipart/form-data; boundary=----VAMeter\r\n");

    /* -------------------------------- Form data ------------------------------- */
    /* --------------------------------- Part 1 --------------------------------- */
    // spdlog::info("send part 1");
    client.println(data.form_data_part_1.c_str());
    // printf("%s\r\n", data.form_data_part_1.c_str());
    std::string().swap(data.form_data_part_1);

    /* -------------------------------- Part file ------------------------------- */
    // spdlog::info("send file");
    uint8_t* fb_buffer_index = fb->buf;
    for (size_t i = 0; i < fb->len; i = i + 1024)
    {
        if (i + 1024 < fb->len)
        {
            client.write(fb_buffer_index, 1024);
            fb_buffer_index += 1024;
        }
        else if (fb->len % 1024 > 0)
        {
            size_t remainder = fb->len % 1024;
            client.write(fb_buffer_index, remainder);
        }
    }

    /* --------------------------------- Part 2 --------------------------------- */
    // spdlog::info("send part 2");
    client.println(data.form_data_part_2.c_str());
    // printf("%s\r\n", data.form_data_part_2.c_str());

    /* -------------------------------------------------------------------------- */
    /*                                  Response                                  */
    /* -------------------------------------------------------------------------- */
    spdlog::info("wait response..");

    // Expose response handler
    class MyHttpClient : public HTTPClient
    {
    public:
        int myHandleHeaderResponse() { return HTTPClient::handleHeaderResponse(); }
    };
    MyHttpClient http;
    http.begin(client, "");

    // Wait and parse response
    HAL::FeedTheDog();
    auto status_code = http.myHandleHeaderResponse();
    data.string_buffer = http.getString().c_str();
    spdlog::info("get code: {}\npayload:\n {}", status_code, data.string_buffer);

    if (status_code != 200)
    {
        spdlog::warn("bad response in code: {}", status_code);
        // return false;
    }

    client.stop();

    // Release buffe
    esp_camera_fb_return(fb);

    return true;
}
