#include "patterns.h"

void warmGlitter(CRGB* leds)
{
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy( leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	leds[pos] += CHSV( 50, 24, 239); // add an LED of warm white
}
