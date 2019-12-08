#include "parser.h"

void posResponse(WiFiClient* client)
{
	// Prepare the response
	String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nCommand received</html>\n";

  // Send the response to the client
	client->print(s);
	return;
}

void negResponse(WiFiClient* client)
{
	Serial.println("invalid request");
	client->print("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>Not found</body></html>");
	return;
}

void checkNewMode(int* pattern, WiFiServer* server)
{
  // Check if a client has connected
	WiFiClient client = server->available();
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

  // Check if this command is about controlling the PUS
	if( req.indexOf("/power/") != -1) 
	{
		if( req.indexOf("/on") != -1)
		{
			digitalWrite(POWER_PIN, HIGH);
		} else if( req.indexOf("/off") != -1)
		{
			digitalWrite(POWER_PIN, LOW);
		} else {
			negResponse(&client);
		}
		posResponse(&client);
    return; // we can't process changing the pattern and the power in one instruction
  }

  // Check if this command is about changing the pattern of the lights
  int patternIdStart = req.indexOf("/pattern/"); 
  if (patternIdStart != -1) {
    // get the id of the pattern out of the request
  	patternIdStart += 9;
  	int patternIdEnd = req.indexOf("/", patternIdStart);
  	*pattern = req.substring(patternIdStart, patternIdEnd).toInt();
  	// The pattern still needs to be range checked or something to ensure that it is a valid pattern

    posResponse(&client);
  } 


  negResponse(&client);

  client.flush();
}
