#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <TimeAlarms.h>
#include <Time.h>
#include <TimeLib.h>
#include "NTPtimeESP.h"

/* LED Display */
// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)
#define NUM_LEDS 30
#define DATA_PIN 1
typedef uint32_t DIGIT[7];
CRGB leds[NUM_LEDS];
const DIGIT numbers[10] =
  {
    {1, 1, 1, 1, 1, 1, 0}, //0
    {0, 1, 1, 0, 0, 0, 0}, //1
    {1, 1, 0, 1, 1, 0, 1}, //2
    {1, 1, 1, 1, 0, 0, 1}, //3
    {0, 1, 1, 0, 0, 1, 1}, //4
    {1, 0, 1, 1, 0, 1, 1}, //5
    {1, 0, 1, 1, 1, 1, 1}, //6
    {1, 1, 1, 0, 0, 0, 0}, //7
    {1, 1, 1, 1, 1, 1, 1}, //8
    {1, 1, 1, 1, 0, 1, 1}  //9
  };
DIGIT digits[4];
char dots[2];
const uint8_t digit_start_adress[4] = {0, 7, 16, 23};
const uint8_t dot_start_adress[2] = {14, 15};
time_t prevDisplay = 0; // when the digital clock was displayed

/* WiFi */
#include "wifi_credentials.h"

/* Software clock */
strDateTime dateTime;
#define DEBUG_ON
NTPtime NTPde("europe.pool.ntp.org");
time_t last_time_sync = 0;
time_t time_sync_intervall = 10; //seconds

void setup()
{
  /* Init Serial */
  Serial.begin(115200);

  /* Init LED prevDisplay */
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  /* Init Wifi */
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  /* Time Sync */
  if (now() - last_time_sync > time_sync_intervall)
  {
    // first parameter: Time zone in floating point (for India); second parameter: 1 for European summer time; 2 for US daylight saving time; 0 for no DST adjustment; (contributed by viewwer, not tested by me)
    dateTime = NTPde.getNTPtime(1.0, 1);
    // check dateTime.valid before using the returned time
    // Use "setSendInterval" or "setRecvTimeout" if required
    if(dateTime.valid){
      setTime(dateTime.hour,dateTime.minute,dateTime.second,dateTime.day,dateTime.month,dateTime.year); // Another way to set
      last_time_sync = now();
      Serial.println("time sync");
    } 
  }
  /* Update LED Display */
  if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      setDisplay(hour()/10, hour()%10, minute()/10, minute()%10);
      Serial.println("display update");
  }      
}

void setDisplay(uint8_t digit_0, uint8_t digit_1, uint8_t digit_2, uint8_t digit_3){
  uint8_t content[4] = {digit_0, digit_1, digit_2, digit_3};
  uint32_t led_color;
  // Digits
  for (int i = 0; i < 4; i++){
    for (int led_segment = 0; led_segment < 7; led_segment++)
    {
      if (numbers[content[i]][led_segment] > 0)
      {
        led_color = CRGB::Red;
      }
      else
      {
        led_color = CRGB::Black;
      }
      leds[digit_start_adress[i] + led_segment] = led_color;
    }
  }
  leds[dot_start_adress[0]] = CHSV(random8(),255,255);
  leds[dot_start_adress[1]] = CHSV(random8(),255,255);
  FastLED.setBrightness(30);
  FastLED.show();
}
