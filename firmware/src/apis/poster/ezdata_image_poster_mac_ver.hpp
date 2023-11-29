/**
 * @file ezdata_image_poster_mac_ver.hpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <esp_camera.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>


static String _http_part_1 = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"dataType\"\r\n\r\nfile\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"watermarkText\"\r\n\r\n";
static String _http_prt_2 = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"name\"\r\n\r\ncaptured\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"permissions\"\r\n\r\n1\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"value\"\r\n\r\ncaptured.jpg\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=\"captured.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
static String _http_tail = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n";


static WiFiClient client;

static bool ezdata_image_poster_mac_ver(String serverName, int serverPort, String serverPath, String nickname) 
{
    bool ret = false;
    String getAll;
    String getBody;
    // WiFiClient client;


    camera_fb_t* fb = NULL;
    fb = esp_camera_fb_get();
    if(!fb) 
    {
        Serial.println("Camera capture failed");    
        printf("Camera capture failed\n");
        // return "啊?";
        return false;
    }
    
    // Serial.println("Connecting to server: " + serverName);

    if (client.connect(serverName.c_str(), serverPort)) 
    {
        // Serial.println("Connection successful!\n");    
        // String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
        // String tail = "\r\n--RandomNerdTutorials--\r\n";


        String head = _http_part_1;
        // head += mac;
        head += nickname;
        head += _http_prt_2;

        String tail = _http_tail;



        uint16_t imageLen = fb->len;
        uint16_t extraLen = head.length() + tail.length();
        uint16_t totalLen = imageLen + extraLen;


        // Serial.printf("img len: %d\nextra len: %d\ntotal len: %d\n", imageLen, extraLen, totalLen);

        
    
        client.println("POST " + serverPath + " HTTP/1.1");
        client.println("Host: " + serverName);
        client.println("Content-Length: " + String(totalLen));

        // Serial.println("POST " + serverPath + " HTTP/1.1");
        // Serial.println("Host: " + serverName);
        // Serial.println("Content-Length: " + String(totalLen));


        // client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
        // client.println();
        // client.print(head);

        

        client.println("Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
        client.println();
        client.print(head);

        // Serial.println("Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
        // Serial.println();
        // Serial.print(head);



    
        uint8_t* fbBuf = fb->buf;
        size_t fbLen = fb->len;
        for (size_t n=0; n<fbLen; n=n+1024) 
        {
            if (n+1024 < fbLen) 
            {
                client.write(fbBuf, 1024);
                // Serial.write(fbBuf, 1024);
                fbBuf += 1024;
            }
            else if (fbLen%1024>0) 
            {
                size_t remainder = fbLen%1024;
                client.write(fbBuf, remainder);
                // Serial.write(fbBuf, remainder);
            }
        }   

        client.print(tail);
        // Serial.print(tail);


        // Serial.println("http done");

        esp_camera_fb_return(fb);
        
        int timoutTimer = 10000;
        long startTimer = millis();
        boolean state = false;
        
        while ((startTimer + timoutTimer) > millis()) 
        {
            // Serial.print(".");
            delay(100);      
            while (client.available()) 
            {
                char c = client.read();
                if (c == '\n') 
                {
                    if (getAll.length()==0) 
                    { 
                        state=true; 
                    }
                    getAll = "";
                }
                else if (c != '\r') 
                { 
                    getAll += String(c); 
                }
                if (state==true) 
                { 
                    getBody += String(c); 
                }
                startTimer = millis();
            }
            if (getBody.length()>0) 
            { 
                break; 
            }
        }
        // Serial.println();

        client.clearWriteError();
        client.stop();

        Serial.println("get body:");
        Serial.println(getBody);
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
