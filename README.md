# ESP32-Arduino-DS18B20_MQTT

Sample readings from a Dallas DS18B20 temperature sensor and publish via MQTT.

## 2025-03-08 Goals

* Refresh my knowledge of VS Code/PlatformIO (PIO) and Arduino model. (I don't care for the Arduino IDE. Further, it seems that the Microsoft Arduino plugin for VS Code is deprecated.)
* Read a Dallas DS18B20 temperature sensor.
* Publish the readings via MQTT

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
