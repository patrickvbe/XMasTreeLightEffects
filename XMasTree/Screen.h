#include <FastLED.h>

// This defines the 'screen' we are using.
// The bottom-left is position (0,0). The top-right is (9,29). So it's like an x/y coordinate system.
#define NUM_STRIPS 10
#define NUM_LEDS 30

class Screen
{
public:
    static constexpr int RowCount() { return NUM_LEDS; }
    static constexpr int ColCount() { return NUM_STRIPS; }
    CRGB* Strip(int col) {return MPixels[col]; }
    CRGB& Pixel(int col, int row) { return MPixels[col][row]; }
    void Clear() { FastLED.clearData(); }
    void DrawGrayscaleImage(const char* img, const CRGB& color=CRGB::Blue);

private:
    CRGB MPixels[NUM_STRIPS][NUM_LEDS];
};
