#include "Screen.h"

void Screen::DrawGrayscaleImage(const char* img, const CRGB& color, int row)
{
  const uint8_t* imgpos = (const uint8_t*)img;
  int rowcount = *imgpos++;
  row = RowCount() - row;
  while ( rowcount-- > 0 )
  {
    row--;
    for ( int col=0; col < ColCount() ; col++)
    {
      Pixel(col, row) = color;
      Pixel(col, row).nscale8(*imgpos++);
    }
  }
}

void Screen::DrawImage(const char* img, int row)
{
  const uint8_t* imgpos = (const uint8_t*)img;
  int rowcount = *imgpos++;
  row = RowCount() - row;
  while ( rowcount-- > 0 )
  {
    row--;
    for ( int col=0; col < ColCount() ; col++)
    {
      int8_t b = *imgpos++;
      int8_t g = *imgpos++;
      int8_t r = *imgpos++;
      Pixel(col, row).setRGB(b, g, r);
    }
  }
}
