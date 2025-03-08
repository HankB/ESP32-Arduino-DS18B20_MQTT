#include <Arduino.h>

const int LED = 2; // Assign LED pin i.e: D1 on NodeMCU

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH); // turn the LED on
  delay(500); // wait for a second
  digitalWrite(LED, LOW); // turn the LED off
  delay(500); // wait for a second
}