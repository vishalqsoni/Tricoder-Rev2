#include "startup.h"

#include "startup/gif_frames.h"
#include "startup/gif_frames.cpp"

#include "startup/sektion.h"

#define FRAME_DELAY 100

void startup(Adafruit_ST7735 &tft)
{

tft.drawRGBBitmap(0, 0, Sektion31sq, 160, 128);

delay(2400);

for(int i = 0; i < gifFrameCount; i++)
{

tft.drawRGBBitmap(0, 0, gifFrames[i], 160, 128);

delay(FRAME_DELAY);

}

}
