#include <FastLED.h>
#include "Screen.h"

// We have only one engine with a maximum number of points. You can configure
// this engine for different pixel effects.
#define MAX_PIXELS 20

// Positions, speeds, etc are expressed in 1/256 pixel units.
// So for example: 0x0100 = 1 unit, 0x0180 = 1.5 unit, 0x0040 = 0.25 unit
// Which limits us to 64 as max value.

// Animate pixel.
class PEPixel
{
public:
  CRGB    color = CRGB::Black;
  int     xpos = 0;
  int     ypos = 0;
  int     xspeed = 0;
  int     yspeed = 0;
  bool    move = false;
};

class PixelEngine
{
public:
  PixelEngine() = delete;
  PixelEngine(Screen& screen) : MScreen(screen)
  {
    MMaxx = ((MScreen.ColCount() - 1) << 8) | 0x80;
    MMaxy = ((MScreen.RowCount() - 1) << 8) | 0x80;
    Reset();
  }
  void Reset();
  void AddPixel(int x, int y, const CRGB& color = CRGB::Black, int xspeed=0, int yspeed=0);
  void ExecuteStep();
  void Draw() const;
  static constexpr int MaxPixels() { return MAX_PIXELS; }

  long MGravity;         // How much does it accelerate (going down) / decelerate (going up) down.
  long MBouncefactor;    // How much speed is left after bouncing.
  bool MCollisionDetection;

private:
  void Bounce(int& pos, int& speed);
  bool Move(int& pos, int& speed, const int max);

  Screen& MScreen;
  int MMaxx;            // Pre-calculated limit in units of 1/256.
  int MMaxy;            // Pre-calculated limit in units of 1/256.
  int MUsedPixels;      // Number of pixels in MPixels which are actually valid / used.
  PEPixel MPixels[MAX_PIXELS];
};
