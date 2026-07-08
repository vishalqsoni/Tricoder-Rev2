#ifndef CAMERA_TFT_H
#define CAMERA_TFT_H

#include <Adafruit_ST7735.h>

void camera_init(Adafruit_ST7735* display);
void camera_start();
void camera_stop();
void camera_loop();

bool camera_isActive();

#endif