#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// ===== MODULES =====
#include "startup/startup.h"
#include "startup/startup.cpp"
#include "led.h"
#include "mcp23008.h"
#include "audio.h"
#include "bme.h"
#include "ui.h"
#include "scanning_run.h"
#include "ina.h"
#include "alert.h"

// ===== UI ASSETS =====
#include "asset_png/lcarsframeblank.h"
#include "asset_png/lcarssplitframe.h"

// ===== TFT =====
#define TFT_CS   4
#define TFT_DC   5
#define TFT_RST  6

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// ===== AUDIO =====
Audio audio(Serial2, 44, 43);

// ===== BUTTON LOCK =====
bool buttonLock[6] = {false};

// ===== FLAGS =====
bool returnedFromScan = true;
bool returnedFromAlert = true;

// ===== INA SMOOTH =====
float smoothI = 0;
float smoothP = 0;

//////////////////////////////////////////////////////////////
// SETUP
//////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);

  Wire.begin(2, 3);  // I2C for INA

  SPI.begin(7, -1, 9, TFT_CS);
  SPI.setFrequency(27000000);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // Modules
  led_init();
  led_setEnabled(true);

  mcp_init();
  audio.begin();

  bme_init();
  bme_setActive(false);

  ina_init();   // do not activate here

  ui_init(tft);
  scanningInit(&tft);
  alertInit(&tft);

  delay(300);
  audio.playStartup();

  startup(tft);

  // Draw main menu
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRGBBitmap(0, 0, lcarsframeblank, 160, 128);
  drawMenu();
}

//////////////////////////////////////////////////////////////
// LOOP
//////////////////////////////////////////////////////////////
void loop()
{
  //////////////////////////////////////////////////////
  // LED ALWAYS RUN
  //////////////////////////////////////////////////////
  if (led_isEnabled()) {
    led_run();
  }

  //////////////////////////////////////////////////////
  // ALERT (HIGHEST PRIORITY)
  //////////////////////////////////////////////////////
  if (alertRun()) {
    returnedFromAlert = false;
    return;
  }

  if (!isAlertActive() && !returnedFromAlert) {
    returnedFromAlert = true;

    tft.fillScreen(ST77XX_BLACK);
    tft.drawRGBBitmap(0, 0, lcarsframeblank, 160, 128);
    drawMenu();
  }

  //////////////////////////////////////////////////////
  // SCANNING
  //////////////////////////////////////////////////////
  if (scanningRun()) {
    returnedFromScan = false;
    return;
  }

  if (!isScanningActive() && !returnedFromScan) {
    returnedFromScan = true;

    tft.fillScreen(ST77XX_BLACK);
    tft.drawRGBBitmap(0, 0, lcarsframeblank, 160, 128);
    drawMenu();
  }

  //////////////////////////////////////////////////////
  // BUTTON INPUT
  //////////////////////////////////////////////////////
  uint8_t state = mcp_read();

  for (int i = 0; i < 6; i++) {

    bool pressed = state & (1 << i);

    if (pressed && !buttonLock[i]) {

      buttonLock[i] = true;
      audio.playClick();

      //////////////////////////////////////////////////////
      // LED TOGGLE
      //////////////////////////////////////////////////////
      if (i == 3) {
        led_setEnabled(!led_isEnabled());
        return;
      }

      //////////////////////////////////////////////////////
      // ALERT TRIGGER
      //////////////////////////////////////////////////////
      if (i == 5) {
        alertStart();
        audio.playAlarm();
        return;
      }

      //////////////////////////////////////////////////////
      // MENU NAVIGATION
      //////////////////////////////////////////////////////
      if (!inSubMenu) {

        if (i == 2) {  // UP
          selectedIndex--;
          if (selectedIndex < 0) selectedIndex = MENU_SIZE - 1;
          drawMenu();
        }

        if (i == 0) {  // DOWN
          selectedIndex++;
          if (selectedIndex >= MENU_SIZE) selectedIndex = 0;
          drawMenu();
        }

        if (i == 1) {  // SELECT

          //////////////////////////////////////////////////////
          // SCAN FROM MENU (INDEX 1)
          //////////////////////////////////////////////////////
          if (selectedIndex == 1) {
            scanningStart();
            audio.playScanning();
            return;
          }

          inSubMenu = true;

          //////////////////////////////////////////////////////
          // BATT SCREEN
          //////////////////////////////////////////////////////
          if (selectedIndex == 5) {
            tft.fillScreen(ST77XX_BLACK);
            tft.drawRGBBitmap(0, 0, lcarssplitframe, 160, 128);
          }
          else {
            drawSubMenu();

            if (selectedIndex >= 2 && selectedIndex <= 4) {
              bme_setActive(true);
            }
          }
        }
      }

      //////////////////////////////////////////////////////
      // BACK FROM SUBMENU
      //////////////////////////////////////////////////////
      else {

        if (i == 0 || i == 2) {

          inSubMenu = false;

          bme_setActive(false);

          tft.fillScreen(ST77XX_BLACK);
          tft.drawRGBBitmap(0, 0, lcarsframeblank, 160, 128);

          drawMenu();
        }
      }
    }

    if (!pressed) buttonLock[i] = false;
  }

  //////////////////////////////////////////////////////
  // INA POWER CONTROL
  //////////////////////////////////////////////////////
  if (inSubMenu && selectedIndex == 5) {
    ina_setActive(true);
  } else {
    ina_setActive(false);
  }

  //////////////////////////////////////////////////////
  // BME SENSOR
  //////////////////////////////////////////////////////
  if (inSubMenu && selectedIndex >= 2 && selectedIndex <= 4) {
    bme_run();
    updateSensorUI();
  }

  //////////////////////////////////////////////////////
  // INA BATTERY SCREEN
  //////////////////////////////////////////////////////
  if (inSubMenu && selectedIndex == 5) {

    ina_run();

    float v = ina_getVoltage();
    float rawI = ina_getCurrent();
    float rawP = ina_getPower();

    // smoothing
    smoothI = rawI * 0.2 + smoothI * 0.8;
    smoothP = rawP * 0.2 + smoothP * 0.8;

    int percent = (v - 3.0) * 100 / (4.2 - 3.0);
    percent = constrain(percent, 0, 100);

    tft.fillRect(30, 35, 120, 80, ST77XX_BLACK);

    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
    tft.setCursor(30, 45);
    tft.print("V: "); tft.print(v, 2); tft.print(" V");

    tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    tft.setCursor(30, 60);
    tft.print("I: "); tft.print(smoothI, 1); tft.print(" mA");

    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.setCursor(30, 75);
    tft.print("P: "); tft.print(smoothP, 1); tft.print(" mW");

    tft.setCursor(30, 90);
    tft.print("BAT: "); tft.print(percent); tft.print("%");

    int barWidth = map(percent, 0, 100, 0, 100);

    uint16_t color = (percent > 60) ? ST77XX_GREEN :
                     (percent > 30) ? ST77XX_YELLOW : ST77XX_RED;

    tft.drawRect(30, 110, 100, 10, ST77XX_WHITE);
    tft.fillRect(30, 110, barWidth, 10, color);

    delay(120);
  }
}