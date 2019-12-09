// This is needed as the ESP fires interrups that can take longer than 50us to process and so
// disrupt the sending of data to the lights 
#define FASTLED_ALLOW_INTERRUPTS 0

#include <FastLED.h>

extern void rainbow(CRGB* leds);