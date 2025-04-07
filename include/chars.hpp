#pragma once
#include <LiquidCrystal_I2C.h>
#include <cstdint>
#include <string>

void loadChars(LiquidCrystal_I2C *lcd);
void printPolishMsg(LiquidCrystal_I2C &lcd, const wchar_t *msg);