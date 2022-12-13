#include <FastLED.h>

#include "PixelEngine.h"

//#define DEBUG
//#define USE_NANO

const int OFF_EFFECTNR = -1;

// Timing. Currently running at app. 50fps.
const unsigned long MS_PER_TICK = 20;
const unsigned long TICKS_PER_SECOND = 1000 / MS_PER_TICK;
const unsigned long AUTOSWITCH_TIMEOUT = 10 * TICKS_PER_SECOND;

// Global status information shared by the various effects
Screen screen;
PixelEngine pixelengine(screen);
int row;
int hue;
int updown = 1; // Controls direction movement

// Global status information for the control loop.
unsigned long ticker = 0;
unsigned long nextswitch = AUTOSWITCH_TIMEOUT;  // Block switching with: nextswitch = ticker
bool globalneweffect = true;
int effectnr = 0;
int number_typed = 0;  // Number (being) typed in with the number keys
unsigned long last_number_key_tick;  // Tick-time last number key was typed.

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); // Also used to communicate UI board.
#ifdef DEBUG
  Serial.println("starting");
#endif
#ifdef USE_NANO
  FastLED.addLeds<WS2812B, 2, GRB>(screen.Strip(0), screen.RowCount());
  FastLED.addLeds<WS2812B, 3, GRB>(screen.Strip(1), screen.RowCount());
  FastLED.addLeds<WS2812B, 4, GRB>(screen.Strip(2), screen.RowCount());
  FastLED.addLeds<WS2812B, 5, GRB>(screen.Strip(3), screen.RowCount());
  FastLED.addLeds<WS2812B, 6, GRB>(screen.Strip(4), screen.RowCount());
  FastLED.addLeds<WS2812B, 7, GRB>(screen.Strip(5), screen.RowCount());
  FastLED.addLeds<WS2812B, 8, GRB>(screen.Strip(6), screen.RowCount());
  FastLED.addLeds<WS2812B, 9, GRB>(screen.Strip(7), screen.RowCount());
  FastLED.addLeds<WS2812B, 10, GRB>(screen.Strip(8), screen.RowCount());
  FastLED.addLeds<WS2812B, 11, GRB>(screen.Strip(9), screen.RowCount());
#else
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
#endif
  FastLED.setBrightness(100);
  FastLED.setDither(0);
}

// Data (images can be (partially) generated with GIMP export functionality)
const char* const rend PROGMEM =
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

void Effect(int new_effectnr)
{
  effectnr = new_effectnr;
  nextswitch = ticker + AUTOSWITCH_TIMEOUT;
  globalneweffect = true;
  screen.Clear();
}

void NextEffect()
{
  Effect(effectnr + 1);
}

void loop() {
  // We read one key every loop cycle, so effects can react on the key variable
  // and don't have to read the keys themselves.
  int key_pressed = -1;
  if ( Serial.available() ) {
    key_pressed = Serial.read();    
    Serial.println(key_pressed, HEX);
    // if ( digitalRead(LED_BUILTIN) == HIGH) digitalWrite(LED_BUILTIN, LOW);
    // else digitalWrite(LED_BUILTIN, HIGH);
  }

  // ======================= Global control keys =======================

  // Let the user type a number to select an effect
  if ( key_pressed >= 0 && key_pressed <= 9 ) // Numeric keys
  {
    number_typed = number_typed * 10 + key_pressed;
    last_number_key_tick = ticker;
  }
  else if ( number_typed != 0 && (key_pressed != -1 || ticker - last_number_key_tick > TICKS_PER_SECOND) )
  {
    Effect(number_typed - 1); // 0 is the 'first' effect so 1 :-)
    number_typed = 0;
  }

  if ( key_pressed == 0x0C ) // Power on /off
  {
    if ( effectnr == OFF_EFFECTNR ) NextEffect(); // Back on.
    else {  // Turn the system 'off'.
      effectnr = OFF_EFFECTNR;
      screen.Clear();
      screen.Pixel(0, screen.RowCount() - 1) = CRGB::DarkRed; // Stand-by light :-)
      screen.Show();
    }
  }
  else if ( key_pressed == 0x54) // Home: Start the normal cyclus
  {
    Effect(0);
  }
  else if ( key_pressed == 0x28) // >> = Next effect
  {
    Effect(effectnr + 1);
  }
  else if ( effectnr > 0 && key_pressed == 0x2B) // << = Previous effect
  {
    Effect(effectnr - 1);
  }

  bool neweffect = globalneweffect;
  globalneweffect = false;

  // ======================= Action! =======================
  switch ( effectnr )
  {
  case OFF_EFFECTNR:
    nextswitch = ticker; // Blocks switching
    break;
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
    if ( neweffect )
    {
      pixelengine.Reset();
      for ( int idx=0; idx < 15; idx++)
      {
        pixelengine.AddPixel(random(Screen::ColCount()),random(Screen::RowCount()),CRGB::OrangeRed, random(100), random(100));
      }
    }
    screen.Clear();
    pixelengine.ExecuteStep();
    pixelengine.Draw();
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
  case 5:
    if ( neweffect )
    {
      pixelengine.Reset();
      for ( int idx=0; idx < 15; idx++)
      {
        pixelengine.AddPixel(random(Screen::ColCount()),random(Screen::RowCount()),CRGB(CHSV(random(50,255), random(50,255), random(50,255))), random(255), random(255));
      }
    }
    screen.Clear();
    pixelengine.ExecuteStep();
    pixelengine.Draw();
    break;
  default:
    // Go back to the start.
    Effect(0);
    break;
  } // switch(effectnr)

  // ======================= Control effect screen + auto switching =======================
  if ( effectnr == OFF_EFFECTNR )
  {
    delay(MS_PER_TICK);
  }
  else
  {
    FastLED.delay(MS_PER_TICK);
    if ( ++ticker == nextswitch ) NextEffect();
  }
}
