#include <FastLED.h>

// How many leds in your strip?
#define NUM_STRIPS 10
#define NUM_LEDS 30

// Define the array of leds
CRGB leds[NUM_STRIPS][NUM_LEDS];

void setup() { 
  FastLED.addLeds<WS2812B, 28, GRB>(leds[0], NUM_LEDS);
  FastLED.addLeds<WS2812B, 29, GRB>(leds[1], NUM_LEDS);
  FastLED.addLeds<WS2812B, 30, GRB>(leds[2], NUM_LEDS);
  FastLED.addLeds<WS2812B, 31, GRB>(leds[3], NUM_LEDS);
  FastLED.addLeds<WS2812B, 32, GRB>(leds[4], NUM_LEDS);
  FastLED.addLeds<WS2812B, 33, GRB>(leds[5], NUM_LEDS);
  FastLED.addLeds<WS2812B, 34, GRB>(leds[6], NUM_LEDS);
  FastLED.addLeds<WS2812B, 35, GRB>(leds[7], NUM_LEDS);
  FastLED.addLeds<WS2812B, 36, GRB>(leds[8], NUM_LEDS);
  FastLED.addLeds<WS2812B, 37, GRB>(leds[9], NUM_LEDS);
  FastLED.setBrightness(100);
}

unsigned char* img =
"\000\000\000\000\000\000\000\000\000\000"
"\000\377\377\377\377\377\377\001\000\000"
"\000\377\377\000\000\000\377\377\000\000"
"\000\377\377\000\000\000\377\377\000\000"
"\000\377\377\000\000\000\377\377\000\000"
"\000\377\377\377\377\377\377\001\000\000"
"\000\377\377\001\377\377\001\000\000\000"
"\000\377\377\000\001\377\377\001\000\000"
"\000\377\377\000\000\001\377\377\001\000"
"\000\377\377\000\000\000\001\377\377\000"
"\000\000\000\000\000\000\000\000\000\000"
"\000\001\377\377\377\001\000\000\000\000"
"\000\377\377\000\377\377\000\000\000\000"
"\000\377\377\000\377\377\000\000\000\000"
"\000\377\377\000\377\377\000\000\000\000"
"\000\001\377\377\377\001\000\377\377\000"
"\000\377\377\001\377\377\001\377\001\000"
"\000\377\377\000\001\377\377\001\000\000"
"\000\377\377\000\000\001\377\377\000\000"
"\000\001\377\377\377\377\001\377\377\000"
"\000\000\000\000\000\000\000\000\000\000"
"\000\377\377\377\377\377\377\001\000\000"
"\000\377\377\000\000\001\377\377\001\000"
"\000\377\377\000\000\000\001\377\377\000"
"\000\377\377\000\000\000\000\377\377\000"
"\000\377\377\000\000\000\000\377\377\000"
"\000\377\377\000\000\000\000\377\377\000"
"\000\377\377\000\000\000\001\377\377\000"
"\000\377\377\000\000\001\377\377\001\000"
"\000\377\377\377\377\377\377\001\000\000";

#define EFFECT01

void loop() {
#ifdef EFFECT01
  unsigned char* imgpos = img;
  for ( int row=NUM_LEDS-1; row >=0; row--)
  {
    for ( int col=NUM_STRIPS-1; col >= 0 ; col--)
    {
      leds[col][row]= CRGB::Red;
      leds[col][row].nscale8(*imgpos++);
    }
  }
  FastLED.delay(500);
#endif
#ifdef EFFECT02
  for ( int lednr=0; lednr < NUM_LEDS; lednr++)
  {
    leds[0][lednr] = CRGB::DarkRed;
    if ( lednr > 0) leds[0][lednr-1] = CRGB::Green;
    if ( lednr > 1) leds[0][lednr-2] = CRGB::Blue;
    if ( lednr > 2) leds[0][lednr-3] = CRGB::Yellow;
    if ( lednr > 3) leds[0][lednr-4] = CRGB::Black;
    for ( int strip=1; strip < NUM_STRIPS; strip++)
    {
      memcpy(leds[strip], leds[strip-1], sizeof(CRGB) * NUM_LEDS);
      for ( int lednr=0; lednr < NUM_LEDS; lednr++)
      {
        leds[strip][lednr].nscale8(150);
      }
    }
    FastLED.delay(20);
  }
#endif
}
