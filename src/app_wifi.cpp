#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "app_wifi.h"

/*
    Associate with a WiFi AP. The ESP32 stores the most recently used
    credentials and in that case the call to WiFi.begin(); should work.

    If there is a need to associate with a different AP or this ESP
    never used WiFi, the correct call is

    const char* ssid = "REPLACE_WITH_YOUR_SSID";
    const char* password = "REPLACE_WITH_YOUR_PASSWORD";
    WiFi.begin(ssid, password);
*/

static const int wifi_retries = 5;

int init_wifi(void)
{
    int retries = wifi_retries;
    while (retries--)
    {
        WiFi.begin();
        if (WiFi.status() == WL_CONNECTED)
            return 0;
    }
    return -1;
}

static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP, "pfsense.localdomain", 0,  (1024*1000));  // In ms, 2^10 seconds, 63 minutes

/*
    initialize NTP client
*/
void init_ntp(void)
{
    timeClient.begin();
    delay(1000); // wait for a second
    timeClient.update();
}

/*
    Update NTP client. It will ignore requests <60s apart.
*/
int update_ntp(void)
{
    return timeClient.update();
}

time_t get_time_t(void)
{
    return timeClient.getEpochTime();
}