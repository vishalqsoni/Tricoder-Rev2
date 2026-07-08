#ifndef ALERT_H
#define ALERT_H

#include <Adafruit_ST7735.h>

void alertInit(Adafruit_ST7735* display);
void alertStart();
bool alertRun();
bool isAlertActive();

#endif
