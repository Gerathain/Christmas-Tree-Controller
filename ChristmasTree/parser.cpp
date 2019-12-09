#include "parser.h"

void checkNewMode(int* pattern, ESP8266WebServer* server)
{
  String newPattern = server->arg("pattern"); // if the pattern arg isn't present then "" is returned
  *pattern = newPattern.toInt(); // This really needs range checking
  return;
}
