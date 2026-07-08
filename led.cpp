#include "led.h"
#include <Adafruit_NeoPixel.h>

#define PIN_1   8
#define PIN_2   1

#define COUNT_1 20
#define COUNT_2 15

Adafruit_NeoPixel strip1(COUNT_1, PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(COUNT_2, PIN_2, NEO_GRB + NEO_KHZ800);

// ===== GLOBAL STATE =====
static bool ledEnabled = true;

// ===== COLORS =====
uint32_t RED, GREEN, BLUE, YELLOW, CYAN, ORANGE;
uint32_t G1, G2, G3;

// ===== TIMERS =====
uint32_t tRed, tBlue, tCyan, tGreen, tBlink1, tBlinkFast, tChaseA, tChaseB, tSeq;

// ===== POSITIONS =====
uint8_t pos12 = 0, pos34 = 2, pos58 = 7, pos912 = 8, pos1318 = 12;
uint8_t pos2427 = 3, pos2830 = 7, dir2830 = 1;

// ===== STATES =====
bool blink1 = 0, fastBlink = 0;
uint8_t seqState = 0;

//////////////////////////////////////////////////////////////
// ================= INIT =================================//
//////////////////////////////////////////////////////////////

void led_init() {

  strip1.begin();
  strip2.begin();

  // Pre-calc colors
  RED    = strip1.Color(255,0,0);
  GREEN  = strip1.Color(0,255,0);
  BLUE   = strip1.Color(0,0,255);
  YELLOW = strip1.Color(255,255,0);
  CYAN   = strip1.Color(0,255,255);
  ORANGE = strip1.Color(255,100,0);

  G1 = strip1.Color(0,180,0);
  G2 = strip1.Color(0,100,0);
  G3 = strip1.Color(0,40,0);

  strip1.show();
  strip2.show();
}

//////////////////////////////////////////////////////////////
// ================= ENABLE CONTROL ========================//
//////////////////////////////////////////////////////////////

void led_setEnabled(bool state) {

  ledEnabled = state;

  if (!ledEnabled) {
    led_off();   // immediately turn OFF
  }
}

bool led_isEnabled() {
  return ledEnabled;
}

//////////////////////////////////////////////////////////////
// ================= MAIN RUN ==============================//
//////////////////////////////////////////////////////////////

void led_run() {

  // 🔥 DO NOTHING IF DISABLED
  if (!ledEnabled) return;

  uint32_t now = millis();

  strip1.clear();
  strip2.clear();

  // ===== STRIP 1 =====

  if (now - tRed >= 1500) { tRed = now; pos12 ^= 1; }
  strip1.setPixelColor(pos12, RED);

  if (now - tBlue >= 1700) { tBlue = now; pos34 = (pos34 == 2) ? 3 : 2; }
  strip1.setPixelColor(pos34, BLUE);

  if (now - tCyan >= 120) {
    tCyan = now;
    pos58 = (pos58 <= 4) ? 7 : pos58 - 1;
    pos912 = (pos912 >= 11) ? 8 : pos912 + 1;
  }
  strip1.setPixelColor(pos58, CYAN);
  strip1.setPixelColor(pos912, CYAN);

  if (now - tGreen >= 120) {
    tGreen = now;
    pos1318 = (pos1318 >= 17) ? 12 : pos1318 + 1;
  }

  for (uint8_t i = 12; i <= 17; i++) {
    uint8_t d = (pos1318 >= i) ? (pos1318 - i) : (pos1318 + 6 - i);

    if (d == 0) strip1.setPixelColor(i, GREEN);
    else if (d == 1) strip1.setPixelColor(i, G1);
    else if (d == 2) strip1.setPixelColor(i, G2);
    else if (d == 3) strip1.setPixelColor(i, G3);
  }

  strip1.setPixelColor(18, YELLOW);
  strip1.setPixelColor(19, YELLOW);

  // ===== STRIP 2 =====

  if (now - tBlink1 >= 1000) { tBlink1 = now; blink1 ^= 1; }
  if (now - tBlinkFast >= 500) { tBlinkFast = now; fastBlink ^= 1; }

  strip2.setPixelColor(0, GREEN);
  strip2.setPixelColor(1, YELLOW);

  if (fastBlink) strip2.setPixelColor(2, RED);

  if (now - tChaseA >= 200) {
    tChaseA = now;
    pos2427 = (pos2427 >= 6) ? 3 : pos2427 + 1;
  }
  strip2.setPixelColor(pos2427, GREEN);

  if (now - tChaseB >= 150) {
    tChaseB = now;
    pos2830 += dir2830;
    if (pos2830 >= 9 || pos2830 <= 7) dir2830 = -dir2830;
  }
  strip2.setPixelColor(pos2830, YELLOW);

  if (blink1) strip2.setPixelColor(10, BLUE);

  if (now - tSeq >= 500) {
    tSeq = now;
    if (seqState < 3) seqState++;
  }

  uint32_t seqColor = (seqState == 1) ? YELLOW :
                      (seqState == 2) ? ORANGE : RED;

  if (seqState) {
    strip2.setPixelColor(11, seqColor);
    strip2.setPixelColor(12, seqColor);
  }

  strip2.setPixelColor(13, RED);
  strip2.setPixelColor(14, CYAN);

  strip1.show();
  strip2.show();
}

//////////////////////////////////////////////////////////////
// ================= ALERT ================================//
//////////////////////////////////////////////////////////////

void led_alert() {

  for (uint8_t i = 0; i < COUNT_1; i++) strip1.setPixelColor(i, RED);
  for (uint8_t i = 0; i < COUNT_2; i++) strip2.setPixelColor(i, RED);

  strip1.show();
  strip2.show();
}

//////////////////////////////////////////////////////////////
// ================= OFF =================================//
//////////////////////////////////////////////////////////////

void led_off() {

  strip1.clear();
  strip2.clear();

  strip1.show();
  strip2.show();
}