#ifndef UI_H
#define UI_H

#include <Adafruit_ST7735.h>

// ===== MENU CONFIG =====
#define MENU_SIZE 6

extern const char* const menuItems[];
extern int selectedIndex;
extern bool inSubMenu;

// ===== UI FUNCTIONS =====
void ui_init(Adafruit_ST7735 &display);
void drawUIOverlay(uint16_t color = ST77XX_YELLOW);
void drawMenu();
void drawSubMenu();
void updateSensorUI();

#endif
