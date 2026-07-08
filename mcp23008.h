#ifndef MCP23008_H
#define MCP23008_H

#include <stdint.h>

// init I2C + MCP
void mcp_init();
//
// read all touch inputs (returns 8-bit state)
uint8_t mcp_read();

// check individual touch (0–7)
bool mcp_isTouched(uint8_t pin);

#endif
