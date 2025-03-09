#include <WiFi.h>

/*
    Associate with a WiFi AP. The ESP32 stores the most recently used
    credentials and in that case the call to WiFi.begin(); should work.

    If there is a need to associate with a different AP or this ESP
    never used WiFi, the correct call is

    const char* ssid = "REPLACE_WITH_YOUR_SSID";
    const char* password = "REPLACE_WITH_YOUR_PASSWORD";
    WiFi.begin(ssid, password);
*/

int init_wifi(void)
{
    return WiFi.begin();
}

#include "app_wifi.h"