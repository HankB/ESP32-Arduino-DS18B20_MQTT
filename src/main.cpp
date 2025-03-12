#include <Arduino.h>
#include "DS18B20.h"
#include "app.h"
#include "app_wifi.h"
#include <stdio.h>
#include <strings.h>

// override serial_IO in app.h
#undef serial_IO
#define serial_IO true

static const int max_sensors = 5;
static DS18B20 sensors[max_sensors];

const int LED = LED_BUILTIN; // Assign LED pin i.e: D1 on NodeMCU

static const char *build_topic(char *buf, int buf_len,
                               const char *location, const char *id);
static const char *build_payload(DS18B20 *s, time_t t);

void setup()
{
  pinMode(LED, OUTPUT);
#if serial_IO
  Serial.begin(115200);
  while (!Serial)
    ; // time to get serial running
#endif
  init_wifi();
  init_ntp();
  init_mqtt("mqtt.localdomain");
}

void loop()
{
  static int sensors_found = 0;
  int rc;
  static time_t epoch = 0;
  static bool already_published = false;

  // check WiFi connection
  while (!is_connected_wifi())
  {
#if serial_IO
    Serial.println("loop() WiFi lost connection");
#endif
    // reconnect_wifi(); needed? autoReconnect is set by default.
    init_wifi();
    delay(1000); // wait for a second
  }
  report_wifi();

  if (~mqtt_is_connected())
    mqtt_reconnect();

  if (0 == sensors_found)
  {
    sensors_found = init_DS18B20(sensors, max_sensors);

    // init tag and topic for the MQTT message
    for (int s = 0; s < sensors_found; s++)
    {
      sensors[s].tag[0] = '\0'; // init string
      for (int i = 0; i < 8; i++)
      {
        int len = strlen(sensors[s].tag);
        snprintf(sensors[s].tag + len, tag_len - len, "%2.2x", sensors[s].addr[i]);
      }
      build_topic(sensors[s].topic, topic_len, "lab", sensors[s].tag);
    }
  }

  digitalWrite(LED, HIGH); // turn the LED on
  for (int i = 0; i < sensors_found; i++)
  {
    sensors[i].temperature = read_DS18B20(&sensors[0]);
  }
  digitalWrite(LED, LOW); // turn the LED off

  update_ntp();
  epoch = get_time_t();

#if serial_IO
  Serial.printf("\t\t\ttime() returns %u\n", epoch);
  Serial.printf("%d sensors", sensors_found);
  for (int i = 0; i < sensors_found; i++)
  {
    Serial.printf(", %f", sensors[i].temperature);
  }
  Serial.println("");

  for (int s = 0; s < sensors_found; s++)
  {
    mqtt_publish(sensors[s].topic, build_payload(&sensors[s], epoch));
  }
#endif

  // wait for 60 seconds, minus typoical time to traverse the loop,
  // 1s for each sensor.
  delay(1000 * (60 - sensors_found)); 
}

/******************************************
 *
 *  build the topic (once) and the payload (every sample)
 *
 *  Topic will be stored in the DS18B20 struct.
 ******************************************/

// topic will look like HA/{hostname}/{location}/{id}
// "HA/ESP01/upstairs/temp.03"
static const char *build_topic(char *buf, int buf_len, const char *location, const char *id)
{
  snprintf(buf, buf_len, "HA/%s/%s/%s", gethostname_wifi(), location, id);
  return buf;
}

// payload will look like
// {"t": 1741749182, "temp": 63.4, "device":"DS18B20", "tag":"284c2e5700000012"}
static const int payload_len = 128;
static char payload[payload_len];

static const char *build_payload(DS18B20 *s, time_t t)
{
  snprintf(payload, payload_len,
           "{\"t\": %u, \"temp\": %.2f, \"device\":\"DS18B20\", \"tag\":\"%s\"}",
           t, s->temperature, s->tag);
  return payload;
}