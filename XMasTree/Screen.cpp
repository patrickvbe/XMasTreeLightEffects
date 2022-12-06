#include "Screen.h"

void Screen::DrawGrayscaleImage(const char* img, const CRGB& color)
{
  const uint8_t* imgpos = (const uint8_t*)img;
  for ( int row=RowCount()-1; row >=0; row--)
  {
    for ( int col=0; col < ColCount() ; col++)
    {
      Pixel(col, row) = color;
      Pixel(col, row).nscale8(*imgpos++);
    }
  }
}
