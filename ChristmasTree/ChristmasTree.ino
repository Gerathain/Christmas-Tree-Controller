// This is needed as the ESP fires interrups that can take longer than 50us to process and so
// disrupt the sending of data to the lights 
#define FASTLED_ALLOW_INTERRUPTS 0

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "WifiSecret.h"
//This has the defines for the wifi ssid and password

#include "parser.h"

// defines for the LEDs
#define DATA_PIN    15
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    50
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

CRGB leds[NUM_LEDS];
int mode = 0;

uint8_t gHue = 0; //base colour for the rainbow TODO remove

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

  // Code for OTA updates
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  // End code for OTA updates
  
  Serial.println("Ready");
}



// rainbow patter for the LEDs
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void loop()
{
  ArduinoOTA.handle();
  
  checkNewMode(&mode, &server); // This is just checking if someone is trying to connect to the server

  if (mode)
  {
    rainbow();
    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND); 

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }
}
