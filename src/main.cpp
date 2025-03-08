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
init_DS18B20(sensors, max_sensors);
}

void loop()
{
  digitalWrite(LED, HIGH); // turn the LED on
  delay(500);              // wait for a second
  digitalWrite(LED, LOW);  // turn the LED off
  delay(500);              // wait for a second
#if serial_IO
  Serial.print("loop\n");
#endif
float temp = read_DS18B20(&sensors[0]);
}