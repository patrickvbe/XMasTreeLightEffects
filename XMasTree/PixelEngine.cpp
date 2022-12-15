#include "PixelEngine.h"

void PixelEngine::Reset()
{
  MGravity = 0;
  MBouncefactor = 0;
  MUsedPixels = 0;
  MCollisionDetection = false;
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

void PixelEngine::Bounce(int& pos, int& speed)
{
  pos -= speed;
  if ( MBouncefactor )
  {
    if ( speed > 0) { if ( (speed -= MBouncefactor) < 0) speed = 0; }
    else            { if ( (speed += MBouncefactor) > 0) speed = 0; }
  }
  speed = -speed;
  pos += speed;
}

bool PixelEngine::Move(int& pos, int& speed, const int max)
{
  bool bounced = false;
  pos += speed;
  if ( pos < 0x80 || pos > max )
  {
    Bounce(pos, speed);
    bounced = true;
  }
  // Some safety guards.
  if ( pos < 0x80) pos = 0x80;
  if ( pos > max) pos = max;
  return bounced;
}

void PixelEngine::ExecuteStep()
{
  for ( int pixidx = 0; pixidx < MUsedPixels; pixidx++)
  {
    PEPixel& pixel = MPixels[pixidx];
    if ( pixel.move )
    {
      int prevx = pixel.xpos;
      int prevy = pixel.ypos;
      bool xbounced = Move(pixel.xpos, pixel.xspeed, MMaxx);
      bool ybounced = Move(pixel.ypos, pixel.yspeed, MMaxy);
      if ( MCollisionDetection )
      {
        int xpos = pixel.xpos >> 8;
        int ypos = pixel.ypos >> 8;
        prevx >>= 8;
        prevy >>= 8;
        if ( (prevx != xpos || prevy != ypos) && MScreen.Pixel(xpos, ypos))  // collision
        { // Check which movement contributed to the colission.
          pixel.color = CRGB::Red;
          if ( !xbounced && prevx != xpos && !MScreen.Pixel(prevx, ypos)) Bounce(pixel.xpos, pixel.xspeed);
          if ( !ybounced && prevy != ypos && !MScreen.Pixel(xpos, prevy)) Bounce(pixel.ypos, pixel.yspeed);
        }
      }
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
