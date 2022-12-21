#include <FastLED.h>

#include "PixelEngine.h"
#include "Images.h"

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
int hue;
int updown = 1; // Controls direction movement
unsigned long next_event_ticks;
char state;

// Global status information for the control loop.
unsigned long ticker = 0;
unsigned long nextswitch = AUTOSWITCH_TIMEOUT;  // Block switching with: nextswitch = ticker
bool globalneweffect = true;
int effectnr = 9;
int number_typed = 0;  // Number (being) typed in with the number keys
unsigned long last_number_key_tick;  // Tick-time last number key was typed.
CRGB colorcorrection;

// Pong!
const int barsize = 3;
const int autospeed = 10;
const int autoplayer_stupid = 10;
const int startspeed = 40;
const int speedincrease = 30; 
int position_bar1;
int position_bar2;
int autospeed_countdown;
int speedincreasecount;
int autodirection = 1;
int gameovercount;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial1.begin(115200); // Communication with the UI board.
#ifdef DEBUG
  Serial.begin(115200);
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
  if ( Serial1.available() ) {
    key_pressed = Serial1.read();    
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
  else if ( key_pressed == 0x10)
  {
    colorcorrection += CRGB(10,0,0);
    FastLED.setCorrection(colorcorrection);
    Serial.print(colorcorrection.red);
  }
  else if ( key_pressed == 0x11)
  {
    colorcorrection -= CRGB(10,0,0);
    FastLED.setCorrection(colorcorrection);
    Serial.print(colorcorrection.red);
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
    pixelengine.ExecuteStep();
    screen.Clear();
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
    pixelengine.ExecuteStep();
    screen.Clear();
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
    pixelengine.ExecuteStep();
    screen.Clear();
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
    pixelengine.ExecuteStep();
    screen.Clear();
    pixelengine.Draw();
    break;
  case 6:
    if ( neweffect )
    {
      next_event_ticks = ticker;
      state = 'r';
    }
    if ( next_event_ticks == ticker )
    {
      switch(state)
      {
      case 'r':
        screen.Clear();
        screen.DrawGrayscaleImage(only_r, CRGB::OrangeRed, 5);
        state = '&';
        break;
      case '&':
        screen.Clear();
        screen.DrawGrayscaleImage(only_and, CRGB::OrangeRed, 5);
        state = 'd';
        break;
      case 'd':
        screen.Clear();
        screen.DrawGrayscaleImage(only_d, CRGB::OrangeRed, 5);
        state = 'r';
        break;
      }
      next_event_ticks = ticker + TICKS_PER_SECOND / 2;
    }
    break;
  case 7:
    if ( neweffect )
    {
      pixelengine.Reset();
      pixelengine.MCollisionDetection = true;
      for ( int idx=0; idx < 15; idx++)
      {
        pixelengine.AddPixel(random(Screen::ColCount()),random(Screen::RowCount()),CRGB::OrangeRed, random(50), random(50));
      }
    }
    // First execute step then clear screen. Te current screen is used to detect collisions.
    pixelengine.ExecuteStep();
    screen.Clear();
    pixelengine.Draw();
    break;
  case 8:
    if ( neweffect )
    {
      pixelengine.Reset();
      pixelengine.MCollisionDetection = true;
      for ( int idx=0; idx < 2; idx++)
      {
        pixelengine.AddPixel(random(Screen::ColCount()),random(Screen::RowCount()),CRGB::OrangeRed, random(50,100), random(50,100));
      }
      nextswitch = ticker + 2000;  // We have our own timeout.
    }
    // First execute step then clear screen. Te current screen is used to detect collisions.
    pixelengine.ExecuteStep();
    screen.Clear();
    screen.DrawGrayscaleImage(blocks, CRGB::White, 12);
    pixelengine.Draw();
    break;
  case 9:
    if ( neweffect )
    {
      screen.DrawImage(smiley,4);
    }
    break;
  case 10:
    // Pong!
    if ( neweffect || (gameovercount != 0 && --gameovercount == 0 ) )
    {
      position_bar1 = 4;
      position_bar2 = 4;
      autospeed_countdown = autospeed;
      speedincreasecount = speedincrease;
      gameovercount = 0;
      pixelengine.Reset();
      pixelengine.MCollisionDetection = true;
      pixelengine.AddPixel(5, 15, CRGB::DarkRed, 10, random(2) > 0 ? startspeed : -startspeed);
    }
    if ( gameovercount == 0 )
    {
      pixelengine.ExecuteStep();
      screen.Clear();
      for ( int x=0; x < barsize; x++ )
      {
        screen.Pixel(position_bar2 + x, screen.RowCount() - 1) = CRGB::White;
        screen.Pixel(position_bar1 + x, 0) = CRGB::White;
      }
      pixelengine.Draw();
      if ( pixelengine.MPixels[0].ypos < (1<<8) || pixelengine.MPixels[0].ypos >= ((screen.RowCount() - 1) << 8) )
      {
        gameovercount = 50;
        for ( int row = 0; row < screen.RowCount() ; row++) for ( int col = 0; col < screen.ColCount(); col++)
        {
          if ( !screen.Pixel(col, row) ) screen.Pixel(col, row) = CRGB::DarkOrange;
        }
      }
      if ( --autospeed_countdown == 0 )
      {
        autospeed_countdown = autospeed;
        autodirection = ( position_bar1 + 1 > pixelengine.MPixels[0].xpos >> 8 ) ? -1 : +1;
        if ( random(autoplayer_stupid) < 1 ) autodirection = - autodirection; // Randomly move in the wrong direction.
        position_bar1 += autodirection;
        if ( position_bar1 < 0 ) position_bar1 = 0;
        if ( position_bar1 > screen.ColCount() - barsize ) position_bar1 = screen.ColCount() - barsize;
      }
      if ( speedincreasecount-- == 0)
      {
        speedincreasecount = speedincrease;
        if ( pixelengine.MPixels[0].yspeed < 255 ) pixelengine.MPixels[0].yspeed++;
      }
      if ( key_pressed != -1 )
      {
        nextswitch = ticker + AUTOSWITCH_TIMEOUT; // Prevents switching while keys are pressed = playing.
        if ( key_pressed == 0x5A)
        {
          if ( position_bar2 > 0 ) position_bar2--;
        }
        if ( key_pressed == 0x5B)
        {
          if ( position_bar2 < screen.ColCount() - barsize) position_bar2++;
        }
      }
    }
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
