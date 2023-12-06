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
#include <HTTPClient.h>
#include <esp_wifi.h>


struct PostCallback_t
{
public:
    virtual void imageCaptured(camera_fb_t* frameBuffer) {}
};


static String _http_part_1 = "------unitcams3poster\r\nContent-Disposition: form-data; name=\"dataType\"\r\n\r\nfile\r\n------unitcams3poster\r\nContent-Disposition: form-data; name=\"watermarkText\"\r\n\r\n";
static String _http_part_2 = "\r\n------unitcams3poster\r\nContent-Disposition: form-data; name=\"timeZoneId\"\r\n\r\n";
static String _http_part_3 = "\r\n------unitcams3poster\r\nContent-Disposition: form-data; name=\"name\"\r\n\r\ncaptured\r\n------unitcams3poster\r\nContent-Disposition: form-data; name=\"permissions\"\r\n\r\n1\r\n------unitcams3poster\r\nContent-Disposition: form-data; name=\"value\"\r\n\r\ncaptured.jpg\r\n------unitcams3poster\r\nContent-Disposition: form-data; name=\"file\"; filename=\"captured.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
static String _http_tail = "\r\n------unitcams3poster--\r\n";


static WiFiClient client;

static bool ezdata_image_poster(String serverName, int serverPort, String serverPath, String nickname, String timeZone, PostCallback_t* postCallback = nullptr) 
{
    bool ret = false;
    String getAll;
    String getBody;

    // Captrue 
    camera_fb_t* fb = NULL;
    fb = esp_camera_fb_get();
    if(!fb) 
    {
        Serial.println("Camera capture failed");    
        printf("Camera capture failed\n");
        // return "啊?";
        return false;
    }

    // Callback 
    if (postCallback != nullptr)
        postCallback->imageCaptured(fb);
    
    // Connect server 
    // Serial.println("Connecting to server: " + serverName);
    if (client.connect(serverName.c_str(), serverPort)) 
    {
        // HTTP message 
        String head = _http_part_1;
        head += nickname;
        head += _http_part_2;
        head += timeZone;
        head += _http_part_3;
        String tail = _http_tail;

        uint16_t imageLen = fb->len;
        uint16_t extraLen = head.length() + tail.length();
        uint16_t totalLen = imageLen + extraLen;
        // Serial.printf("img len: %d\nextra len: %d\ntotal len: %d\n", imageLen, extraLen, totalLen);

        // Start sending 
        client.println("POST " + serverPath + " HTTP/1.1");
        client.println("Host: " + serverName);
        client.println("Content-Length: " + String(totalLen));
        // Serial.println("POST " + serverPath + " HTTP/1.1");
        // Serial.println("Host: " + serverName);
        // Serial.println("Content-Length: " + String(totalLen));

        client.println("Content-Type: multipart/form-data; boundary=----unitcams3poster");
        client.println();
        client.print(head);
        // Serial.println("Content-Type: multipart/form-data; boundary=----unitcams3poster");
        // Serial.println();
        // Serial.print(head);

        // Image data 
        uint8_t* fbBuf = fb->buf;
        size_t fbLen = fb->len;
        for (size_t n = 0; n < fbLen; n = n + 1024) 
        {
            if (n + 1024 < fbLen) 
            {
                client.write(fbBuf, 1024);
                // Serial.write(fbBuf, 1024);
                fbBuf += 1024;
            }
            else if (fbLen % 1024 > 0) 
            {
                size_t remainder = fbLen % 1024;
                client.write(fbBuf, remainder);
                // Serial.write(fbBuf, remainder);
            }
        }   

        client.print(tail);
        // Serial.print(tail);


        // Release buffe 
        esp_camera_fb_return(fb);
        

        // Wait result 
        int timoutTimer = 10000;
        long startTimer = millis();
        boolean state = false;
        while ((startTimer + timoutTimer) > millis()) 
        {
            delay(100);      
            while (client.available()) 
            {
                char c = client.read();
                if (c == '\n') 
                {
                    if (getAll.length()==0) 
                    { 
                        state = true; 
                    }
                    getAll = "";
                }
                else if (c != '\r') 
                { 
                    getAll += String(c); 
                }
                if (state == true) 
                { 
                    getBody += String(c); 
                }
                startTimer = millis();
            }
            if (getBody.length() > 0) 
            { 
                break; 
            }
        }

        // Stop client 
        client.clearWriteError();
        client.stop();

        // Log result 
        Serial.printf("get body: %s\n", getBody.c_str());
        delay(100);
        printf("%s\n", getBody.c_str());
        ret = true;
    }
    else 
    {
        getBody = "Connection to " + serverName +  " failed.";
        Serial.println(getBody);
        delay(100);
        printf("%s\n", getBody.c_str());
        ret = false;
    }
    return ret;
}
