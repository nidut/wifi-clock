# wifi-clock
A 3D printed, wifi connected clock with seven segment look for the living room.

Hardware:
- NodeMCU board with ESP8266 controller
- WS2812B LED strip
- 5V supply

Software:
- FastLED https://github.com/FastLED/FastLED
- NTPtimeESP https://github.com/SensorsIot/NTPtimeESP

Functionality:
- time display in HH:MM
- time sync with NTP server
- software clock based on microcontroller clock (without real-time clock)
- automatic daylight saving time
- RGB color

Todo:
- auto brightness
- date display
- timer mode
- case
- integrated USB Hub
- control interface