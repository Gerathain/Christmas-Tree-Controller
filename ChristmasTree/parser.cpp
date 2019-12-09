#include "parser.h"

String s = "Command received";

void checkNewMode(int* pattern, ESP8266WebServer* server)
{
  Serial.println("Pattern changing");
  String newPattern = server->arg("pattern"); // if the pattern arg isn't present then "" is returned
  server->sendHeader("Connection", "close");
  server->send(200, "text/html", s);
  
  Serial.print("new pattern: ");
  Serial.println(newPattern);
  *pattern = newPattern.toInt(); // This really needs range checking
  return;
}
