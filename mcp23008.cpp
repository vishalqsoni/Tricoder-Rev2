#include "mcp23008.h"
#include <Wire.h>

#define MCP_ADDR 0x20

// MCP23008 Registers
#define IODIR   0x00
#define GPIO    0x09
#define GPPU    0x06
//
uint8_t lastState = 0;

void mcp_init() {
  Wire.begin(2, 3); // SDA = GPIO2, SCL = GPIO3

  // Set all pins as INPUT
  Wire.beginTransmission(MCP_ADDR);
  Wire.write(IODIR);
  Wire.write(0xFF);   // all input
  Wire.endTransmission();

  // Enable pull-ups (optional but recommended)
  Wire.beginTransmission(MCP_ADDR);
  Wire.write(GPPU);
  Wire.write(0xFF);
  Wire.endTransmission();
}

uint8_t mcp_read() {
  Wire.beginTransmission(MCP_ADDR);
  Wire.write(GPIO);
  Wire.endTransmission();

  Wire.requestFrom(MCP_ADDR, 1);
  if (Wire.available()) {
    lastState = Wire.read();
  }

  return lastState;
}

bool mcp_isTouched(uint8_t pin) {
  if (pin > 7) return false;

  // TTP223 → HIGH when touched
  return (lastState & (1 << pin));
}
