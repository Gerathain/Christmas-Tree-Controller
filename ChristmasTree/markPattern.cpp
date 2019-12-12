#include "patterns.h"

#define SLOWNESS 1
#define SKIP_VALUE (random8(5) + 2)
#define COLOR_VALUE random8()

int ledindex = 0;
int skip = SKIP_VALUE;
int color = COLOR_VALUE;
int maxlevel = SLOWNESS * skip;
int level = maxlevel;
CRGB oldcolor;

void markPattern(CRGB* leds)
{
    if (level == 0) {
        ledindex += skip;
        if (ledindex > (NUM_LEDS - 1)) {
            color = COLOR_VALUE;
            skip = 0 - (SKIP_VALUE);
            maxlevel = SLOWNESS * (0 - skip);
            while (ledindex > (NUM_LEDS - 1)) ledindex += skip;
        } else if (ledindex < 0) {
            color = COLOR_VALUE;
            skip = SKIP_VALUE;
            maxlevel = SLOWNESS * skip;
            while (ledindex < 0) ledindex += skip;
        }
        oldcolor = leds[ledindex];
        level = maxlevel + 1;
    } else {
        level -= 1;
        leds[ledindex] = blend(CHSV(color, 255, 255), oldcolor, (level*255)/maxlevel);
    }
}
