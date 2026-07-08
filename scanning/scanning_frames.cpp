#include "scanning/scanning_frames.h"

static Adafruit_ST7735* tft = nullptr;

static bool active = false;
static uint32_t startTime = 0;
static uint32_t lastFrameTime = 0;
static uint8_t frameIndex = 0;

#define FRAME_DELAY 40
#define SCAN_DURATION 20000

//////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////
void scanningInit(Adafruit_ST7735* display)
{
  tft = display;
}

//////////////////////////////////////////////////////////////
// START
//////////////////////////////////////////////////////////////
void scanningStart()
{
  if (!tft) return;

  active = true;
  startTime = millis();
  lastFrameTime = 0;
  frameIndex = 0;
}

//////////////////////////////////////////////////////////////
// RUN
//////////////////////////////////////////////////////////////
bool scanningRun()
{
  if (!active || !tft) return false;

  uint32_t now = millis();

  // draw frame
  if (now - lastFrameTime >= FRAME_DELAY) {
    lastFrameTime = now;

    tft->drawRGBBitmap(
      0,
      0,
      scanningFrames[frameIndex],
      160,
      128
    );

    frameIndex++;
    if (frameIndex >= SCAN_FRAME_COUNT) frameIndex = 0;
  }

  // stop after time
  if (now - startTime >= SCAN_DURATION) {
    active = false;
  }

  return active;
}

//////////////////////////////////////////////////////////////
// STATE CHECK
//////////////////////////////////////////////////////////////
bool isScanningActive()
{
  return active;
}
