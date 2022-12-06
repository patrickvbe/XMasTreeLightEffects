#include <FastLED.h>

#include "PixelEngine.h"

//#define DEBUG

// 500 is app. 10s
#define AUTOSWITCH_TIMEOUT 500

// Global status information shared by the various effects
Screen screen;
PixelEngine pixelengine(screen);
int row;
int hue;
int updown = 1; // Controls direction movement

// Global status information for the control loop.
unsigned long ticker = 0;
unsigned long nextswitch = AUTOSWITCH_TIMEOUT;  // Block switching with: nextswitch = ticker
bool neweffect = true;
int effectnr = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("starting");
#endif
  FastLED.addLeds<WS2812B, 37, GRB>(screen.Strip(0), screen.RowCount());
  FastLED.addLeds<WS2812B, 36, GRB>(screen.Strip(1), screen.RowCount());
  FastLED.addLeds<WS2812B, 35, GRB>(screen.Strip(2), screen.RowCount());
  FastLED.addLeds<WS2812B, 34, GRB>(screen.Strip(3), screen.RowCount());
  FastLED.addLeds<WS2812B, 33, GRB>(screen.Strip(4), screen.RowCount());
  FastLED.addLeds<WS2812B, 32, GRB>(screen.Strip(5), screen.RowCount());
  FastLED.addLeds<WS2812B, 31, GRB>(screen.Strip(6), screen.RowCount());
  FastLED.addLeds<WS2812B, 30, GRB>(screen.Strip(7), screen.RowCount());
  FastLED.addLeds<WS2812B, 29, GRB>(screen.Strip(8), screen.RowCount());
  FastLED.addLeds<WS2812B, 28, GRB>(screen.Strip(9), screen.RowCount());
  FastLED.setBrightness(25);
  FastLED.setDither(0);
}

// Data (images can be (partially) generated with GIMP export functionality)
const char* rend =
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

void NextEffect()
{
  if ( ++effectnr > 4 ) effectnr = 0;
  nextswitch = ticker + AUTOSWITCH_TIMEOUT;
  neweffect = true;
  screen.Clear();
}

void loop() {
  switch ( effectnr )
  {
  case 0:
    if ( neweffect )
    {
      screen.DrawGrayscaleImage(rend);
    }
    break;
  case 1:
    if ( neweffect )
    {
      hue = 0;
    }
    screen.DrawGrayscaleImage(rend, CRGB(CHSV(hue, 0xFF, 0xFF)));
    if ( hue >= 0xFF ) updown = -1;
    if ( hue < updown ) updown = 1;
    hue += updown;
    break;
  case 2:
    // We effectively have a continuous for loop with the 'row' variable.
    if ( neweffect )
    {
      row = 0;
    }
    screen.Pixel(0, row) = CRGB::DarkRed;
    if ( row > 0) screen.Pixel(0, row-1) = CRGB::Green;
    if ( row > 1) screen.Pixel(0, row-2) = CRGB::Blue;
    if ( row > 2) screen.Pixel(0, row-3) = CRGB::Yellow;
    if ( row > 3) screen.Pixel(0, row-4) = CRGB::Black;
    for ( int col=1; col < screen.ColCount(); col++)
    {
      for ( int row=0; row < NUM_LEDS; row++)
      {
        screen.Pixel(col, row) = screen.Pixel(0,row);
        screen.Pixel(col, row).nscale8(150);
      }
    }
    if ( ++row >= screen.RowCount() ) row = 0;
    break;
  case 3:
    if ( neweffect )
    {
      pixelengine.Reset();
      pixelengine.AddPixel(5,7,CRGB::White, 30, 100);
      pixelengine.AddPixel(9,25,CRGB::Red, 100, 100);
      pixelengine.AddPixel(1,20,CRGB::Blue, 100, 100);
      pixelengine.MBouncefactor = 10;
      pixelengine.MGravity = 5;
      nextswitch = ticker + 1000;  // We have our own timeout.
    }
    screen.Clear();
    pixelengine.ExecuteStep();
    pixelengine.Draw();
    break;
  case 4:
    if ( neweffect )
    {
      pixelengine.Reset();
      for ( int idx=0; idx < 15; idx++)
      {
        pixelengine.AddPixel(random(Screen::ColCount()),random(Screen::RowCount()),CRGB::OrangeRed, random(255), random(255));
      }
    }
    screen.Clear();
    pixelengine.ExecuteStep();
    pixelengine.Draw();
    break;
  } // switch(effectnr)

  FastLED.delay(20); // Running at app. 50fps.

  // Control effect switching.
  neweffect = false;
  if ( ++ticker == nextswitch ) NextEffect();
}
