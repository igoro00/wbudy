#pragma once
#include "PolishChars.h"
#include <cstdint>
#include "hardware/i2c.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/structs/io_bank0.h"
#include "hardware/regs/addressmap.h"
#include "hardware/structs/padsbank0.h"

class WbudyLCD {
public:
    WbudyLCD(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t sda, uint8_t scl);

    WbudyLCD();
    void init(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t sda, uint8_t scl);
    
    void clear();
    void setCursor(uint8_t row, uint8_t col);
    void print(const char* str);
    void setBacklight(bool val);

private:
    i2c_inst_t* _i2c;
    uint8_t _addr;
    uint8_t _sda;
    uint8_t _scl;

    void sendData(uint8_t data, uint8_t mode);
    void sendCmd(uint8_t cmd);
    void writeByte(uint8_t data);
    void sendChar(char c);
    void toggleEnable(uint8_t data);
    void sleep_ms_custom(uint32_t ms);
    void set_pin_function_i2c(uint pin);
    void set_pin_pullup(uint pin);
    void loadCustomChar(uint8_t location, const uint8_t charmap[8]);
    uint8_t utf8BytesCounter(uint8_t c);
    const wchar_t walkUTF8String(const uint8_t** pStr);
    uint8_t mapPolishChar(const uint8_t** pStr);
    bool _backlight = true;
};
