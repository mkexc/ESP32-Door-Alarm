# ESP32 DOOR ALARM

This project has been made only from a **hobby** perspective.

## Description
This project aims to give you a modern and complete IoT door alarm to deploy on your esp32. 
It has been built with esp32-arduino framework.

It is based on the Magnetic Hall effect, which is why a magnet mounted on the door is required. 
You can adapt it with a proxymity sensor if you want, but it will not be as accurate as this.  
When the door opens, magnetic field decreases triggering the KY-024 hall sensor.   
If the alarm is armed it sends an email to your email address configured in the code every 60 seconds, until the door closes or you unarm the alarm.
When alarm is armed, the LED turns ON.
It includes also an alarm manager accessible from [ESP32IP's]:80 (or with a DDNS) with a switch button.  
Remember to configure in the code your SMTP,WiFi,EasyDDNS and Timezone settings.

## Hardware
- ESP32 DevKit
- Generic LED(w/ 220 Ω resistor)
- KY-024 Magnetic Hall sensor
- Little Magnet

## Dependencies

- ESP32-Arduino framework
- [NTPClient](https://github.com/taranais/NTPClient) Library (by taranais)
- [ESP32 Mail Client](https://github.com/mobizt/ESP32-Mail-Client) Library (by mobizt)
- [EasyDDNS](https://github.com/ayushsharma82/EasyDDNS) Library (by ayushsharma82)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) Library (by me-no-dev)

If you use PlatformIO you can install them directly from the integrated library manager, except for NTPClient which you should install manually because it's a fork.
Same for Arduino IDE.

## Changelog
- 1.0: Initial release
- 1.1: Alarm Manager is now a standalone webapp, with full iOS favicon,app icon and splashscreen support.  
(Android soon/not tested, as I don't have an Android device right now.)  
Save to homescreen to get the standalone webapp.
- 1.2: Polished iOS PWA. Changed Partition scheme with a custom made one.

## Compiling in Arduino IDE

- Rename Alarm.cpp from *.cpp to *.ino
- Remove function prototypes
- Remove "#include <Arduino.h>"
- Install deps
- Insert personal Data(WiFi,SMTP etc...) into Code
- Upload Firmware
- Upload Data

## Compiling in VSCode

- Install PlatformIO
- Configure platform.ini with your local ESP32's IP address (if you wan OTA Upload)
- Install deps
- Insert personal Data(WiFi,SMTP etc...) into Code
- Upload Firmware
- Upload File System Image

## Circuit Example

<img src="https://github.com/mkexc/ESP32_DoorAlarm/raw/master/common/images/Circuit.png" width="400">


## Credits

**Author**: Michelangelo Bartolomucci (**mkexc**)  
If you liked this project and want to support further development, you can buy me a [🍻beer](https://www.paypal.me/mibart/5). :)  
**License GPLv3**