#include <ESP8266WiFi.h>

#include "WifiSecret.h"
//This has the defines for the wifi ssid and password

#include "parser.h"
#include "patterns.h"

// defines for the LEDs
#define DATA_PIN    15
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

// This is the array of all of the possible patterns
void (* patterns[])(CRGB*) = {rainbow, warmGlitter};
CRGB leds[NUM_LEDS];
int pattern = 0;

void setupWifi()
{
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(MY_SSID);

  WiFi.begin(MY_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  return;
}

void setupLEDs()
{
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void setup()
{
  Serial.begin(115200);

  setupWifi();
  setupLEDs();

  // initialise the pin for turning the PSU on/off
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
  
  Serial.println("Ready");
}

void loop()
{  
  checkNewMode(&pattern, &server); // This is just checking if someone is trying to connect to the server

  patterns[pattern](leds);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}
