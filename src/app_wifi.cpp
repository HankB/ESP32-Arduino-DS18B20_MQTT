#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
<<<<<<< HEAD
#include <PubSubClient.h>
=======
>>>>>>> 61fc2a1 (WiFi improved, NTP working)

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
        WiFi.begin();
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
    Update NTP client.
    It seems to ignore requests < autoReconnect seconds apart.
*/
int update_ntp(void)
{
    return timeClient.update();
}

time_t get_time_t(void)
{
    return timeClient.getEpochTime();
}

/******************************************

    Manage the MQTT connection and publish messages.

    NB: This is coded to work on a local LAN and with
        a broker that has password authentication
        disabled. The broker in use is Mosquitto on
        Debian (Raspberry Pi) The README has notes for
        how to disable password authentication for
        Mosquitto. Search for 'allow_anonymous` and
        Mosquitto.

        The example code at 
        https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_basic/mqtt_basic.ino
        provides the inspiration for the MQTT code below.
        No callback() is provided because there is no 
        current plan to subscribe to MQTT messages.

******************************************/

static WiFiClient espClient;
static PubSubClient client(espClient);
static unsigned long lastMsg = 0;

// override serial_IO in app.h
#undef serial_IO
#define serial_IO true

void init_mqtt(const char* broker)
{
    client.setServer(broker, 1883);
}

void mqtt_reconnect(void)
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP32-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str()))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

bool mqtt_is_connected(void)
{
    return client.connected();
}

bool mqtt_publish(const char *topic, const char *payload)
{
    return client.publish(topic, payload);
}