#pragma once
#include <cstdint>

void setLED(byte r, byte g, byte b);
void setLEDByUID(uint32_t uid);
uint8_t simpleHash(uint32_t input);
uint32_t HSL2RGB(uint8_t h, uint8_t s, uint8_t l);