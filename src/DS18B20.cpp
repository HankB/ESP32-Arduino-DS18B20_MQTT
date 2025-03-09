/*
    Code extracted from the sample provided by the PIO library
    and heavily mangled to meet local needs.
*/

#include <Arduino.h>
#include <OneWire.h>

#include "DS18B20.h"
#include "app.h"

OneWire ds(4); // on pin 4 (a 2.6K resistor is used for the ESP)

/*
    ID available DS18B20 sensors
    return the number identified.
*/
int init_DS18B20(DS18B20 *sensors, int max_sensors)
{
    int count = 0;
    int rc;

    ds.reset_search();
    delay(250);

    while (count < max_sensors)
    {
        rc = ds.search(sensors[count].addr);
        if (0 == rc)
        {
            return count;
        }
#if serial_IO
        Serial.print("ROM =");
        for (int i = 0; i < 8; i++)
        {
            Serial.print(sensors[count].addr[i], HEX);
        }

        if (OneWire::crc8(sensors[count].addr, 7) !=
            sensors[count].addr[7])
        {
            Serial.println("CRC is not valid!");
            continue;
        }
        Serial.println();

        // the first ROM byte indicates which chip
        switch (sensors[count].addr[0])
        {
        case 0x10:
            Serial.println("  Chip = DS18S20"); // or old DS1820
            sensors[count].type_s = 1;
            break;
        case 0x28:
            Serial.println("  Chip = DS18B20");
            sensors[count].type_s = 0;
            break;
        case 0x22:
            Serial.println("  Chip = DS1822");
            sensors[count].type_s = 0;
            break;
        default:
            Serial.println("Device is not a DS18x20 family device.");
            continue;
        }

#else
        if (OneWire::crc8(sensors[count].addr, 7) !=
            sensors[count].addr[7])
        {
            continue;
        }
        // the first ROM byte indicates which chip
        switch (sensors[count].addr[0])
        {
        case 0x10:
            sensors[count].type_s = 1;
            break;
        case 0x28:
            sensors[count].type_s = 0;
            break;
        case 0x22:
            sensors[count].type_s = 0;
            break;
        default:
            continue;
        }

#endif

        count++; // look for next DS18B20
    }
    return 0;
}

float read_DS18B20(DS18B20 *s)
{
    byte i;
    byte present = 0;
    byte data[9];
    float celsius, fahrenheit;

    ds.reset();
    ds.select(s->addr);
    ds.write(0x44, 1); // start conversion, with parasite power on at the end

    delay(1000); // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(s->addr);
    ds.write(0xBE); // Read Scratchpad

#if serial_IO
    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
#endif
    for (i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = ds.read();
#if serial_IO
        Serial.print(data[i], HEX);
        Serial.print(" ");
#endif
    }
#if serial_IO
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();
#endif
    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (s->type_s)
    {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    }
    else
    {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20)
            raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40)
            raw = raw & ~1; // 11 bit res, 375 ms
                            //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    fahrenheit = celsius * 1.8 + 32.0;
#if serial_IO
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
#endif

    return fahrenheit;
}