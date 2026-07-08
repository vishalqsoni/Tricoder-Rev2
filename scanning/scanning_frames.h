#ifndef SCANNING_RUN_H
#define SCANNING_RUN_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// init once in setup
void scanningInit(Adafruit_ST7735* display);

// start scanning
void scanningStart();

// run in loop (returns true if active)
bool scanningRun();

// check state
bool isScanningActive();

#endif
