#include "bme.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <math.h>

static Adafruit_BME280 bme;

static bool bmeActive = false;
static unsigned long lastBME = 0;

static float pressureValue = 0;
static float temperatureValue = 0;
static float altitudeValue = 0;

// ===== INIT =====
void bme_init() {

  if (!bme.begin(0x76)) {
    bme.begin(0x77);
  }

  // Forced mode → ultra low power
  bme.setSampling(
    Adafruit_BME280::MODE_FORCED,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::FILTER_OFF
  );
}

// ===== ENABLE / DISABLE =====
void bme_setActive(bool state) {
  bmeActive = state;
}

// ===== RUN =====
void bme_run() {

  // 🔥 HARD STOP
  if (!bmeActive) return;

  unsigned long now = millis();
  if (now - lastBME < 2000) return;

  lastBME = now;

  // one-shot measurement
  bme.takeForcedMeasurement();

  temperatureValue = bme.readTemperature();
  pressureValue = bme.readPressure() * 0.01f;

  altitudeValue = 44330.0 * (1.0 - pow(pressureValue / 1013.25, 0.1903));
}

// ===== GETTERS =====
float bme_getPressure() { return pressureValue; }
float bme_getTemperature() { return temperatureValue; }
float bme_getAltitude() { return altitudeValue; }