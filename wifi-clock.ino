// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "wifi_credentials.h"
// modify wifi_crendentials_example.h and save it as wifi_credentials.h

/* LED Strip */
#define LED_PIN   D1
#define LED_COUNT 30
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

/* LED 7-segment Display */
typedef uint32_t DIGIT[7];
// display with 4 digits and two dots
struct display {
    DIGIT digits[4];  
    char dots[2];
    int brightness = 50;
    uint32_t color = COLOR_RED;
    int updateRate = 500; // in ms
} myDisplay;
// define digits 0-9
DIGIT numbers[10] =
{
    {1,1,1,1,1,1,0},    //0
    {0,1,1,0,0,0,0},    //1
    {1,1,0,1,1,0,1},    //2
    {1,1,1,1,0,0,1},    //3
    {0,1,1,0,0,1,1},    //4
    {1,0,1,1,0,1,1},    //5
    {0,0,1,1,1,1,1},    //6
    {1,1,1,0,0,0,0},    //7
    {1,1,1,1,1,1,1},    //8
    {1,1,1,1,0,1,1}     //9
};

/* predefined colors */
const uint32_t COLOR_OFF = strip.Color(0, 0, 0);
const uint32_t COLOR_RED = strip.Color(255, 0, 0);
const uint32_t COLOR_GREEN = strip.Color(0, 255, 0);
const uint32_t COLOR_BLUE = strip.Color(0, 0, 255);
const uint32_t COLOR_WHITE = strip.Color(255, 255, 255);
const uint32_t COLOR_MAGENTA = strip.Color(255, 0, 255);

/* NTP Client */
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const int CET_OFFSET = 3600;
const int CEST_OFFSET = 7200;
int time_digits[4];
int date_digits[4];
int timeZoneUpdateRate = 3600; // in seconds

uint32_t loop_cnt = 0;

void setup() {
  /* Init LEDs */  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  
  /* Init Serial */
  Serial.begin(115200);
  
  /* Init Wifi */
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  /* Init NTPClient */
  timeClient.begin();
}

void loop() {
  if(loop_cnt*myDisplay.updateRate/1000 % timeZoneUpdateRate == 0)
  {
    // update TimeZone every hour
    updateTimeZone();
  }
  // update time_digits and date_digits
  updateDateTime();
  // set the display according to the current time
  setTimeDisplay();
  // print debug info to serial monitor
  printDebug();
  // set the rest of the display
  myDisplay.color = COLOR_RED;
  myDisplay.brightness = 50;
  // write to display
  writeDisplay(myDisplay);
  delay(myDisplay.updateRate);
  loop_cnt++;
}

void updateDateTime()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String formattedDate = timeClient.getFormattedDate();
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  Serial.println(formattedDate);
  // Extract date
  date_digits[0] = formattedDate[8]-'0';
  date_digits[1] = formattedDate[9]-'0';
  date_digits[2] = formattedDate[5]-'0';
  date_digits[3] = formattedDate[6]-'0';
  // Extract time
  time_digits[0] = formattedDate[11]-'0';
  time_digits[1] = formattedDate[12]-'0';
  time_digits[2] = formattedDate[14]-'0';
  time_digits[3] = formattedDate[15]-'0';
} 
void setTimeDisplay()
{
  memcpy(myDisplay.digits[0], numbers[time_digits[0]], sizeof(DIGIT));
  memcpy(myDisplay.digits[1], numbers[time_digits[1]], sizeof(DIGIT));
  memcpy(myDisplay.digits[2], numbers[time_digits[2]], sizeof(DIGIT));
  memcpy(myDisplay.digits[3], numbers[time_digits[3]], sizeof(DIGIT));
  // set the rest of the display
  myDisplay.dots[0] = 1;
  myDisplay.dots[1] = 1;
}
void setDateDisplay()
{  
  memcpy(myDisplay.digits[0], numbers[date_digits[0]], sizeof(DIGIT));
  memcpy(myDisplay.digits[1], numbers[date_digits[1]], sizeof(DIGIT));
  memcpy(myDisplay.digits[2], numbers[date_digits[2]], sizeof(DIGIT));
  memcpy(myDisplay.digits[3], numbers[date_digits[3]], sizeof(DIGIT));
  // set the rest of the display
  myDisplay.dots[0] = 0;
  myDisplay.dots[1] = 1;
}
uint32_t reduceBrightness(uint32_t color, uint8_t brightness)
{
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color >> 0);
  r =  r * brightness / 255;
  g =  g * brightness / 255;
  b =  b * brightness / 255;
  uint32_t new_color = strip.Color(r,g,b);
  return new_color;
}

void writeDisplay(struct display myDisplay)
{
  strip.clear();
  // Digits
  for(int i = 0; i<4;i++)
  {
    for(int j = 1; j<8;j++)
    {
      if(myDisplay.digits[i][j-1] > 0)
      {
        strip.setPixelColor(i*7+j-1, reduceBrightness(myDisplay.color, myDisplay.brightness));
      }
    }
  }
  // Dots
  for(int i = 0; i<2; i++)
  {
      if(myDisplay.dots[i] > 0)
      {
        strip.setPixelColor(28+i, reduceBrightness(myDisplay.color, myDisplay.brightness));
      }
  }
  strip.show();
}

void updateTimeZone()
{
  timeClient.setTimeOffset(0);
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String utc_date = timeClient.getFormattedDate();
  if (isEuropeanSummerTime(utc_date))
  {
    timeClient.setTimeOffset(CEST_OFFSET);
  }
  else
  {
    timeClient.setTimeOffset(CET_OFFSET);
  }
}

bool isEuropeanSummerTime(String formattedDate)
{
  // check if Central European Summer Time or not
  // input in UTC
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  int year = formattedDate.substring(0, 4).toInt();
  int month = formattedDate.substring(5, 7).toInt();
  int day = formattedDate.substring(8, 10).toInt();
  int hour = formattedDate.substring(11, 13).toInt();
  
  switch (month)  
  { 
  case 1 ... 2:
    // January or February
    return false;
  case 3:
    // March
    int startday = (31-((((5*year)/4)+4)%7));
    if (day > startday || (day==startday && hour >= 1))
    {
      // Summer time has already started
      return true;
    }
    break;
  case 4 ... 9: 
    // April until September
    return true;
  case 10:
    // October
    int endday = (31-((((5*year)/4)+1)%7));
    if (day < endday || (day == endday && hour < 1))
    {
      // Summer time has not ended yet
      return true;
    }
    break;
  case 11 ... 12: 
    // November or December
    return false;
  default: 
    // only for invalid month outside 1..12
    return false;
  }
  // Beginning of March or end of October
  return false;
}
void printDebug()
{
  // Debug time
  Serial.print("Time:");
  for (int i=0; i<4;i++)
  {
    for(int j=0; j<7;j++)
    {
     Serial.print(myDisplay.digits[i][j]); 
    }
    Serial.print(" ");
  }
  Serial.println("");
}