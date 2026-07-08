#ifndef INA_H
#define INA_H

// ===== INIT =====
void ina_init();

// ===== CONTROL =====
void ina_setActive(bool state);

// ===== TASK =====
void ina_run();

// ===== GETTERS =====
float ina_getVoltage();   // Bus voltage (V)
float ina_getCurrent();   // Current (mA)
float ina_getPower();     // Power (mW)

#endif