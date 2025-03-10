#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "app_wifi.h"

/******************************************
    Associate with a WiFi AP. The ESP32 stores the most recently used
    credentials and in that case the call to WiFi.begin(); should work.

    If there is a need to associate with a different AP or this ESP
    never used WiFi, the correct call is

    const char* ssid = "REPLACE_WITH_YOUR_SSID";
    const char* password = "REPLACE_WITH_YOUR_PASSWORD";
    WiFi.begin(ssid, password);
******************************************/

static const int wifi_retries = 5;

int init_wifi(void)
{
    int retries = wifi_retries;
    wl_status_t st;

    while (retries--)
    {
        WiFi.begin("Nacho24", "Nachos Rule 89765433");
        if ((st = WiFi.status()) == WL_CONNECTED)
        {
#if serial_IO
            Serial.printf("init_wifi() IP address: ");
            Serial.println(WiFi.localIP());
#endif
            return 0;
        }
        else
        {
#if serial_IO
            Serial.printf("retry WiFi init_wifi() %d\n", st);
#endif
            delay(250);
        }
    }
#if serial_IO
    Serial.printf("No WiFi init_wifi() %d\n", st);
#endif
    return -1;
}

bool is_connected_wifi(void)
{
    return WiFi.status() == WL_CONNECTED;
}

bool reconnect_wifi(void)
{
    int retries = wifi_retries;
    while (retries--)
    {
        if (WiFi.reconnect())
        {
#if serial_IO
            Serial.printf("reconnect_wifi() IP address: ");
            Serial.println(WiFi.localIP());
#endif
            return true;
        }
    }
    return false;
}

void report_wifi(void)
{
    wl_status_t st = WiFi.status();
    if (st == WL_CONNECTED)
    {
#if serial_IO
        Serial.printf("report_wifi() IP address: ");
        Serial.println(WiFi.localIP());
#endif
    }
#if serial_IO
    else
    {
        Serial.printf("report_wifi() not associated: %d\n", st);
    }
#endif
}

/******************************************

    Use the WiFi connection to update time via NTP.
    May need to specify your router IP (if it serves NTP)
    or specify the FQDN of an NTP server (pool.ntp.org)
    In my LAN where the router is named `pfsense`,
    'pfsense.localdomain` works (but not `pfsense`)
    "192.168.0.1" (IP address of my routrer) also works.

    Per https://github.com/arduino-libraries/NTPClient/issues/185
    I have changed the update interval. As near as I can tell
    the code can call update() as often as it wants and updates
    only actually occur on the specified interval.

******************************************/
static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP, "192.168.1.1", 0, (1024 * 1000)); // In ms, 2^10 seconds, 63 minutes

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