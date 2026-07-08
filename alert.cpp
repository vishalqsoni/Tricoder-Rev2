#include "alert.h"
#include "asset_png/lcarssplitframe.h"

static Adafruit_ST7735* tft = nullptr;

static bool active = false;
static uint32_t startTime = 0;
static uint32_t lastUpdate = 0;

static uint8_t currentLine = 0;
static uint8_t charIndex = 0;

#define ALERT_DURATION 7000
#define CHAR_DELAY 50

// ===== TEXT =====
const char* lines[] = {
  "CORE INSTABILITY",
  "POWER LEVEL CRITICAL",
  "INITIATE CONTAINMENT"
};

#define TOTAL_LINES 3

//////////////////////////////////////////////////////////////
void alertInit(Adafruit_ST7735* display)
{
  tft = display;
}

//////////////////////////////////////////////////////////////
void alertStart()
{
  if (!tft) return;

  active = true;
  startTime = millis();
  lastUpdate = 0;

  currentLine = 0;
  charIndex = 0;

  // draw alert background
  tft->drawRGBBitmap(0, 0, lcarssplitframe, 160, 128);

  tft->setTextSize(1);
  tft->setTextColor(ST77XX_GREEN, ST77XX_BLACK);
}

//////////////////////////////////////////////////////////////
bool alertRun()
{
  if (!active || !tft) return false;

  uint32_t now = millis();

  // ===== DRAW TEXT =====
  if (now - lastUpdate >= CHAR_DELAY) {
    lastUpdate = now;

    // clear text area only
    tft->fillRect(20, 35, 140, 60, ST77XX_BLACK);

    int y = 40;

    // completed lines
    for (int i = 0; i < currentLine; i++) {
      tft->setCursor(20, y + i * 15);
      tft->print(lines[i]);
    }

    // current line typing
    tft->setCursor(20, y + currentLine * 15);

    for (int i = 0; i < charIndex; i++) {
      tft->print(lines[currentLine][i]);
    }

    charIndex++;

    // move next line
    if (lines[currentLine][charIndex] == '\0') {
      charIndex = 0;
      currentLine++;

      if (currentLine >= TOTAL_LINES) {
        currentLine = 0;  // loop text
      }
    }
  }

  // ===== STOP AFTER TIME =====
  if (now - startTime >= ALERT_DURATION) {
    active = false;
  }

  return active;
}

//////////////////////////////////////////////////////////////
bool isAlertActive()
{
  return active;
}
