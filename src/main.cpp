#include <Arduino.h>
#include "DS18B20.h"

static const int max_sensors = 5;
static DS18B20 sensors[max_sensors];

const int LED = LED_BUILTIN; // Assign LED pin i.e: D1 on NodeMCU
// OneWire  ds(4);  // on pin 4 (a 4.7K resistor is necessary)

#define serial_IO true

void setup()
{
  pinMode(LED, OUTPUT);
#if serial_IO
  Serial.begin(9600);
  while (!Serial)
    ; // time to get serial running
#endif
}

void loop()
{
  int sensors_found = init_DS18B20(sensors, max_sensors);

  digitalWrite(LED, HIGH); // turn the LED on
  delay(500);              // wait for a second
  digitalWrite(LED, LOW);  // turn the LED off
  delay(500);              // wait for a second
  float temp = read_DS18B20(&sensors[0]);
#if serial_IO
Serial.printf("%d sensors, first is %f\n", sensors_found, temp);
  
#endif
}