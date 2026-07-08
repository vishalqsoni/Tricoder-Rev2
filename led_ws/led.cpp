#include "led.h"
#include <Adafruit_NeoPixel.h>

#define PIN_1   8
#define PIN_2   1

#define COUNT_1 20
#define COUNT_2 15

Adafruit_NeoPixel strip1(COUNT_1, PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(COUNT_2, PIN_2, NEO_GRB + NEO_KHZ800);

// Colors
uint32_t RED, GREEN, BLUE, YELLOW, CYAN, ORANGE;
uint32_t G1, G2, G3;

// States + timers
uint32_t t1,t2,t3,t4,t5,t6,t7,t8,t9;
uint8_t p1=0,p2=2,p3=7,p4=8,p5=12;
uint8_t p6=3,p7=7,dir=1;
bool b1=0,b2=0;
uint8_t seq=0;

void led_init() {
  strip1.begin();
  strip2.begin();

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

// 👉 YOUR MAIN PATTERN (paste full logic here)
void led_run() {

  uint32_t now = millis();

  strip1.clear();
  strip2.clear();

  // Example (you paste your full optimized logic here)
  if (now - t1 >= 1000) {
    t1 = now;
    b1 ^= 1;
  }

  if (b1)
    strip2.setPixelColor(10, BLUE);

  strip1.show();
  strip2.show();
}

// Simple alert
void led_alert() {
  for (int i=0;i<COUNT_1;i++)
    strip1.setPixelColor(i, RED);

  for (int i=0;i<COUNT_2;i++)
    strip2.setPixelColor(i, RED);

  strip1.show();
  strip2.show();
}

// Turn off LEDs
void led_off() {
  strip1.clear();
  strip2.clear();
  strip1.show();
  strip2.show();
}
