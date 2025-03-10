#include <Arduino.h>
#include "DS18B20.h"
#include "app.h"
#include "app_wifi.h"

// override serial_IO in app.h
#undef serial_IO
#define serial_IO true

static const int max_sensors = 5;
static DS18B20 sensors[max_sensors];

const int LED = LED_BUILTIN; // Assign LED pin i.e: D1 on NodeMCU

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
}

void loop()
{
  static int sensors_found = 0;
  int rc;
  static time_t epoch = 0;

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

  if (0 == sensors_found)
    sensors_found = init_DS18B20(sensors, max_sensors);

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

#endif

  delay(1000); // wait for a second
}