#ifndef LED_H
#define LED_H

#include <stdint.h>

// ===== INIT =====
void led_init();

// ===== MAIN LOOP =====
void led_run();

// ===== CONTROL =====
void led_setEnabled(bool state);
bool led_isEnabled();

// ===== MODES =====
void led_alert();

// ===== OFF =====
void led_off();

#endif