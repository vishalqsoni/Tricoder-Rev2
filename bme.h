#ifndef BME_H
#define BME_H

void bme_init();
void bme_setActive(bool state);
void bme_run();

float bme_getPressure();
float bme_getTemperature();
float bme_getAltitude();

#endif