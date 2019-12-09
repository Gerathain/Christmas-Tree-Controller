#include "patterns.h" 

uint8_t gHue = 0; //base colour for the rainbow

void rainbow(CRGB* leds)
{
	// FastLED's built-in rainbow generator
	fill_rainbow( leds, NUM_LEDS, gHue, 7);

	// do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}
