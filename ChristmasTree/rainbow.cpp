#include "patterns.h" 

void rainbow(CRGB* leds)
{
	// FastLED's built-in rainbow generator
	fill_rainbow( leds, NUM_LEDS, gHue, 7);
}