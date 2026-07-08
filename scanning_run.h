#ifndef SCANNING_RUN_H
#define SCANNING_RUN_H

#include <Adafruit_ST7735.h>

void scanningInit(Adafruit_ST7735* display);
void scanningStart();
bool scanningRun();
bool isScanningActive();

#endif