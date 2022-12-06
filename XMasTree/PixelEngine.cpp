#include "PixelEngine.h"

void PixelEngine::Reset()
{
  MGravity = 0;
  MBouncefactor = 0;
  MUsedPixels = 0;
}

void PixelEngine::AddPixel(int x, int y, const CRGB& _color, int _xspeed, int _yspeed)
{
  if ( MUsedPixels < MAX_PIXELS )
  {
    PEPixel& pixel = MPixels[MUsedPixels++];
    pixel.xpos = x << 8;
    pixel.ypos = y << 8;
    pixel.color = _color;
    pixel.xspeed = _xspeed;
    pixel.yspeed = _yspeed;
    pixel.move = _xspeed != 0 || _yspeed != 0;
  }
}

void PixelEngine::Move(int& pos, int& speed, const int max)
{
  pos += speed;
  if ( pos > max ) // Speed must be positive now.
  {
    pos -= speed;
    if ( (speed -= MBouncefactor) < 0) speed = 0;
    speed = -speed;
    pos += speed;
  }
  if ( pos < 0x80 )  // Speed must be negative now.
  {
    pos -= speed;
    if ( (speed += MBouncefactor) > 0 ) speed = 0;
    speed = -speed;
    pos += speed;
  }
  // Some safety guards.
  if ( pos < 0x80) pos = 0x80;
  if ( pos > max) pos = max;
}

void PixelEngine::ExecuteStep()
{
  for ( int pixidx = 0; pixidx < MUsedPixels; pixidx++)
  {
    PEPixel& pixel = MPixels[pixidx];
    if ( pixel.move )
    {
      Move(pixel.xpos, pixel.xspeed, MMaxx);
      Move(pixel.ypos, pixel.yspeed, MMaxy);
      pixel.yspeed -= MGravity; // We have a simple liniair gravity :-)
    }
  }
}

void PixelEngine::Draw() const
{
  for ( int pixidx = 0; pixidx < MUsedPixels; pixidx++)
  {
    const PEPixel& pixel = MPixels[pixidx];
    // We could support fractions of units and spread them over multiple (max 4?) pixels.
    MScreen.Pixel(pixel.xpos >> 8, pixel.ypos >> 8) = pixel.color;
  }
}
