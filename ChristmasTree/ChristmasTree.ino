#include <ESP8266WiFi.h>

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

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
ESP8266WebServer server(80);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
const char* host = "esp8266-webupdate";

// This is the array of all of the possible patterns
void (* patterns[])(CRGB*) = {rainbow, warmGlitter, markPattern};
CRGB leds[NUM_LEDS];
int pattern = 0;

// This wrapper is basically just needed to pass these variables into the mode checker
void parseConnection()
{
  checkNewMode(&pattern, &server);
  return;
}

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

  MDNS.begin(host);
  
  // Handle the default connection for people to upload
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

  // Handle people connecting to change the pattern
  server.on("/pattern", HTTP_GET, parseConnection);

  // Handle the posting of updates
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });
  server.begin();
  MDNS.addService("http", "tcp", 80);
  
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
  server.handleClient(); // This handles the updating of the code and handling connections to change the current pattern

  patterns[pattern](leds);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}
