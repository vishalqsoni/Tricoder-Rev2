#include "ui.h"
#include "bme.h"
#include <math.h>

// ===== UI ASSETS =====
#include "asset_png/lcarsframeblank.h"
#include "asset_png/backgraph.h"
#include "asset_png/background.h"
#include "asset_png/lcarsburgerframe.h"

// ===== MENU STATE =====
const char* const menuItems[] = {
  "CAMERA",
  "SCAN",
  "PRESS",
  "ALT",
  "TEMP",
  "BATT"
};

int selectedIndex = 0;
bool inSubMenu = false;

// ===== INTERNAL REFERENCE =====
static Adafruit_ST7735* _tft = nullptr;

void ui_init(Adafruit_ST7735 &display) {
  _tft = &display;
}

//////////////////////////////////////////////////////////////
// ================= ALT UI OVERLAY ========================//
//////////////////////////////////////////////////////////////
void drawUIOverlay(uint16_t color) {

  if (!_tft) return;

  _tft->setFont(NULL);
  _tft->setTextSize(1);
  _tft->setTextColor(color, ST77XX_BLACK);

  _tft->setCursor(11, 117);
  _tft->print("A");

  _tft->setCursor(76, 117);
  _tft->print("OK");

  _tft->setCursor(126, 117);
  _tft->print("BACK");
}

//////////////////////////////////////////////////////////////
// ================= MENU ================================//
//////////////////////////////////////////////////////////////
void drawMenu() {

  if (!_tft) return;

  _tft->fillRect(40, 10, 110, 110, ST77XX_BLACK);

  _tft->setFont(NULL);
  _tft->setTextColor(ST77XX_CYAN, ST77XX_BLACK);

  int prev = (selectedIndex - 1 + MENU_SIZE) % MENU_SIZE;
  int next = (selectedIndex + 1) % MENU_SIZE;

  _tft->setTextSize(1);
  _tft->setCursor(45, 30);
  _tft->print(menuItems[prev]);

  _tft->setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  _tft->setTextSize(2);
  _tft->setCursor(45, 55);
  _tft->print(menuItems[selectedIndex]);
  

 
  _tft->setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  _tft->setTextSize(1);
  _tft->setCursor(45, 90);
  _tft->print(menuItems[next]);
}

//////////////////////////////////////////////////////////////
// ================= SUBMENU ==============================//
//////////////////////////////////////////////////////////////
void drawSubMenu() {

  if (!_tft) return;

  _tft->fillScreen(ST77XX_BLACK);

  if (selectedIndex == 2) {
    _tft->drawRGBBitmap(0, 0, backgraph, 160, 128);
    drawUIOverlay(ST77XX_YELLOW);
  }
  else if (selectedIndex == 3) {
    _tft->drawRGBBitmap(0, 0, lcarsburgerframe, 160, 128);
    drawUIOverlay(ST77XX_YELLOW);
  }
  else if (selectedIndex == 4) {
    _tft->drawRGBBitmap(0, 0, background, 160, 128);
  }
}

//////////////////////////////////////////////////////////////
// ================= SENSOR UI ============================//
//////////////////////////////////////////////////////////////
void updateSensorUI() {

  if (!_tft) return;

  static float lastPressure = -9999;
  static float lastAlt = -9999;
  static float lastTemp = -9999;

  float value;
  float* lastPtr;

  if (selectedIndex == 2) {
    value = bme_getPressure();
    lastPtr = &lastPressure;
  }
  else if (selectedIndex == 3) {
    value = bme_getAltitude();
    lastPtr = &lastAlt;
  }
  else if (selectedIndex == 4) {
    value = bme_getTemperature();
    lastPtr = &lastTemp;
  }
  else return;

  // update only if changed → no flicker
  if (fabs(value - *lastPtr) < 0.2) return;
  *lastPtr = value;

  _tft->fillRect(30, 50, 120, 30, ST77XX_BLACK);

  _tft->setTextSize(2);
  _tft->setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  _tft->setCursor(30, 50);

  if (selectedIndex == 2) {
    _tft->print(value, 1);
    _tft->print(" hPa");
  }
  else if (selectedIndex == 3) {
    _tft->print(value, 1);
    _tft->print(" m");
  }
  else {
    _tft->print(value, 1);
    _tft->print(" C");
  }
}
