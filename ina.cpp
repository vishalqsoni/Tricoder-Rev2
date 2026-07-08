#include "ina.h"
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Arduino.h>

// ===== INA OBJECT =====
static Adafruit_INA219 ina;

// ===== STATE =====
static bool inaActive = false;
static uint32_t lastRead = 0;

// ===== STORED VALUES =====
static float voltage = 0;
static float current = 0;
static float power   = 0;

//////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////
void ina_init()
{
  ina.begin();   // default address 0x40
}

//////////////////////////////////////////////////////////////
// ENABLE / DISABLE
//////////////////////////////////////////////////////////////
void ina_setActive(bool state)
{
  inaActive = state;
}

//////////////////////////////////////////////////////////////
// RUN (NON-BLOCKING)
//////////////////////////////////////////////////////////////
void ina_run()
{
  if (!inaActive) return;

  uint32_t now = millis();

  // read every 200 ms (good balance)
  if (now - lastRead < 200) return;
  lastRead = now;

  voltage = ina.getBusVoltage_V();
  current = ina.getCurrent_mA();
  power   = ina.getPower_mW();

  // optional debug
  /*
  Serial.print("V: "); Serial.print(voltage);
  Serial.print(" I: "); Serial.print(current);
  Serial.print(" P: "); Serial.println(power);
  */
}

//////////////////////////////////////////////////////////////
// GETTERS
//////////////////////////////////////////////////////////////
float ina_getVoltage() { return voltage; }
float ina_getCurrent() { return current; }
float ina_getPower()   { return power; }