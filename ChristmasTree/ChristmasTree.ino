#define FASTLED_ALLOW_INTERRUPTS 0

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include "WifiSecret.h"


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

void setup() {
  Serial.begin(115200);

  setupWifi();
  setupLEDs();
}

void checkNewMode() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  unsigned long timeout = millis() + 3000;
  while (!client.available() && millis() < timeout) {
    delay(1);
  }
  if (millis() > timeout) {
    Serial.println("timeout");
    client.flush();
    client.stop();
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  if (req.indexOf("/gpio/0") != -1) {
    mode = 0;
    // clear the lights
    FastLED.clear();
    FastLED.show();
  } else if (req.indexOf("/gpio/1") != -1) {
    mode = 1;
  } else {
    Serial.println("invalid request");
    client.print("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>Not found</body></html>");
    return;
  }

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nThe christmas tree is now ";
  s += (mode) ? "on" : "off";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

// rainbow patter for the LEDs
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void loop() {
  checkNewMode();

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
