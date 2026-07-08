#include "scanning_run.h"
#include "scanning_frames.h"

static Adafruit_ST7735* tft = nullptr;

static bool active = false;
static uint32_t startTime = 0;
static uint32_t lastFrameTime = 0;
static uint8_t frameIndex = 0;

// 🔥 Increased delay (important optimization)
#define FRAME_DELAY 40
#define SCAN_DURATION 16000

void scanningInit(Adafruit_ST7735* display)
{
  tft = display;
}

void scanningStart()
{
  if (!tft) return;

  active = true;
  startTime = millis();
  lastFrameTime = 0;
  frameIndex = 0;
}

bool scanningRun()
{
  if (!active || !tft) return false;

  uint32_t now = millis();

  if (now - lastFrameTime >= FRAME_DELAY) {
    lastFrameTime = now;

    // FULL SCREEN DRAW
    tft->drawRGBBitmap(
      0,
      0,
      scanningFrames[frameIndex],
      SCAN_WIDTH,
      SCAN_HEIGHT
    );

    frameIndex++;
    if (frameIndex >= SCAN_FRAME_COUNT) frameIndex = 0;
  }

  if (now - startTime >= SCAN_DURATION) {
    active = false;
  }

  return active;
}

bool isScanningActive()
{
  return active;
}