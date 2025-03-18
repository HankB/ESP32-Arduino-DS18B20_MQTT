# ESP32-Arduino-DS18B20_MQTT

Sample readings from a Dallas DS18B20 temperature sensor and publish via MQTT.

## 2025-03-08 Goals

* Refresh my knowledge of VS Code/PlatformIO (PIO) and Arduino model. (I don't care for the Arduino IDE. Further, it seems that the Microsoft Arduino plugin for VS Code is deprecated.)
* Read a Dallas DS18B20 temperature sensor.
* Publish the readings via MQTT

## Status

* 2025-03-08 Toolchain installed and working.
* 2025-03-08 DS18B20 example code working.
* 2025-03-08 DS18B20 refactor in progress.
* 2025-03-08 DS18B20 refactor complete, serial output conditional\
* 2025-03-08 WiFi associates
* 2025-03-09 NTP
* 2025-03-11 MQTT
* 2025-03-11 Publish temperature at desired interval.
* 2025-03-11 first revision is complete.

## TODO

Code is functioning and usable as is, but some things could be improved. Lots of parameters are hard coded like

* MQTT Broker (Same for all installs on a site.) It could be arbitrarily set to "mqtt.localdomain" as long as the host with that name is the one runing the broker.
* location (in topic) - specific to each sensor.
* last component in topic as well as ID in "tag" in the payload are the 1-Wire device ID.
* SSID and password are hard coded and stored in EEPROM. There are facilities to allow the device to be configured either by acting as an AP or using Bluetooth. But these are also constant for a site.

Location and "tag" could be mapped to the 1-Wire device ID and this could be hard coded in a table in the application. Better yet, the 

## 2025-03-08 Getting started

1. Install the PlatformIO IDE in VS Code. (currently V 3.3.4)
1. Connect an ESP32 Wroom board to the desktop using USB. (Note:development on Debian Bookworm.)
1. Review quick tutoroal at <https://www.youtube.com/watch?v=nlE2203Q3XI>
1. Clone this repo and try to create the PIO/Arduino project here. PIO pops a warning and creates in `~/Programming/PlatformIO/Projects/ESP32-Arduino-DS18B20_MQTT` instead of `~/Programming/ESP32-Arduino-DS18B20_MQTT` as desired.
1. Copy files from `~/Programming/PlatformIO/Projects/ESP32-Arduino-DS18B20_MQTT` to `~/Programming/ESP32-Arduino-DS18B20_MQTT`

```text
hbarta@olive:~/Programming/ESP32-Arduino-DS18B20_MQTT$ cp -a ~/Programming/PlatformIO/Projects/ESP32-Arduino-DS18B20_MQTT/* .
hbarta@olive:~/Programming/ESP32-Arduino-DS18B20_MQTT$ 
```

6. Open the directory in VS Code and click the PIO button.
1. Find that the board I chose is not found. Find an old PIO/ESP32 and copy the appropriate lines from `platformio.ini`:

```text
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
```

8. PIO does some stuff and asks that I upgrade to version 6.1.17. Current is 3.3.4. The VS Code management page has Auto Update checked. Ah... I think the message means the ESP32 support <https://github.com/platformio/platform-espressif32/releases> I go to View | Command Palette and enter `platformio upgrade` and PIO opens a console window where I see:

```text
pio upgrade
hbarta@olive:~/Programming/ESP32-Arduino-DS18B20_MQTT$ pio upgrade
Please wait while upgrading PlatformIO ...
PlatformIO has been successfully upgraded to 6.1.17
Release notes: https://docs.platformio.org/en/latest/history.html
Warning! Please restart IDE to affect PIO Home changes
hbarta@olive:~/Programming/ESP32-Arduino-DS18B20_MQTT$ 
```

Ahhh... Toolchain installed. I think. But no source code. Need to blink an LED to prove everything is working.

Find my ESP8266 Blinky project <https://github.com/HankB/Blinky>, establish that the ESP32 uses the same GPIO for the on bopard LED (2) and copy the C++ source file. Build, flash and Voilà - a blinking blue LED! Now to get to work.

Pull the repo to another directory, change blink frequency, build and flash to confirm that everything is in the repo that needs to be in the repo.

## 2025-03-08 Serial I/O

Useful for debugging. Again looking at <https://github.com/HankB/ESP8266-BME280-Publish/blob/main/src/main.cpp> for an example. This project also has some of the other stuff like MQTT and NTP that will be useful.

Copy/paste some code for serial and it works. (NB: PlatformIO runs the serial monnotor at 9600 8-N-1 so code set to match.)

## 2025-03-08 DS18B20

Ref: <https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/>

1. Open PIO Home tab and click the "Libraries" button on the left margin. Search for "onewire" and click the top choice ("OneWire by Paul Stoffregen")
1. Click "Add to Project" and select this project.
1. Examples open - select " DS18x20_Temperature"
1. Copy/paste code from `loop()` to project. Build and afdter adding definition for `ds` the project builds. 
1. Now need to connect a DS18B20 to the ESP. Will use the "Normal" mode wiring and a TO-92 package DS18B20.

## 2025-03-08 wiring and test

Code says 10 but example from <https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/> uses GPIO 4 so switching that. Found a 5.6K resistor and used that for the pullup. No joy. Get repeated:

```text
No more addresses.

No more addresses.

No more addresses.
```

Library provides a link to <https://www.pjrc.com/teensy/td_libs_OneWire.html> and there is a note:


> OneWire requires a single 4.7K pullup resistor, connected between the pin and your power supply. When using very long wires, or with counterfeit DS18B20 chips and 3.3V power, a resistor in the 1K to 2.7K range may be required. 

Since this is being powered by a 3V3 ESP, trying a 2.6K resistor and now see:

```text
ROM = 28 D5 27 56 0 0 0 49
  Chip = DS18B20
  Data = 1 53 1 7F 80 7F FF D 10 66  CRC=66
  Temperature = 21.19 Celsius, 70.14 Fahrenheit
loop
No more addresses.
```

Great success!

Add a 2nd sensor:

```text
ROM = 28 4C 2E 57 0 0 0 12
  Chip = DS18B20
  Data = 1 50 1 7F 80 7F FF 10 10 C6  CRC=C6
  Temperature = 21.00 Celsius, 69.80 Fahrenheit
loop
ROM = 28 D5 27 56 0 0 0 49
  Chip = DS18B20
  Data = 1 58 1 7F 80 7F FF 8 10 76  CRC=76
  Temperature = 21.50 Celsius, 70.70 Fahrenheit
loop
No more addresses.
```

`ROM` is the sensor address seen on the Raspberry Pi. These can be mapped to tags (in the future.)

## 2025-03-08 refactor DS18B20 code

* Provide an `init_DS18B20()` that will identify all available sensors and save their addresses.
* Provide a `read_DS18B20(addr)` that will read a specified sensor and return the reading.

## 2025-03-09 add WiFi

Include the header WiFi.h and it Just Works. (NB: This ESP has associated with my AP previously so it already has working credentials.) WiFi API docs at <https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html>

Added `wifi.cpp` and `app_wifi.h` to use for WiFi related code including NTP and MQTT.

## 2025-03-09 NTP client

Use the first library that came up in the search `NTPClient by Fabrice Weinberg`. See also <https://github.com/arduino-libraries/NTPClient>

## 2025-03-10 MQTT

Using `PubSubClient by Nick O'Leary` (and which I've used before.) Github: <https://github.com/knolleary/pubsubclient> Docs: <https://pubsubclient.knolleary.net/>

Question: Do we open the connection to the broker and hold it or do we open the connection when needed and then close it immediately thereafter? Present code opens the connection on every loop and it seems to time out shortly thereafter.

## 2025-03-10 testing

Not a lot of formal testing has been performed, mostly just see "if it works." Some testing has been performed to verify potential WiFi issues.

* Power down AP and then power it back up: ESP reconnects.
* Power down AP, reset ESP ("EN" button near USB port), power up AP: ESP eventually reconnects.

Following tests with a TEST AP, the ESP is configured to associate with the IoT LAN.

Include the header WiFi.h and it Just Works. (NB: This ESP has associated with my AP previously so it already has working credentials.)

## Errata

* 2025-03-08 PIO complains that it cannot find the `go` command during startup. I ignore this.
* 2025-03-08 PIO suggests adding `udev` rules but seems to download w/out difficulty. (ignoring for now.)

```text
Warning! Please install `99-platformio-udev.rules`. 
More details: https://docs.platformio.org/en/latest/core/installation/udev-rules.html
```

* 2025-05-10 [debugging no-associate on Android hotspot](https://github.com/espressif/arduino-esp32/issues/2110#issuecomment-2711021058). Debugging info at <https://docs.espressif.com/projects/arduino-esp32/en/latest/guides/core_debug.html?highlight=logging> (I have no plans to switch to the `pioarduino` for this project.)

* 2025-03-16 PIO required `python3-venv` on Debian Trixie (Testing.) Also required the `udev` rules to connect to the ESP.
