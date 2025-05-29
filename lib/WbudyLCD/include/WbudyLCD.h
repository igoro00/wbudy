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
    
    void initLCD();
    void clear();
    void setCursor(uint8_t row, uint8_t col);
    void print(const char* str);
    void printPolish(const wchar_t* str);
    void backlightOn();
    void backlightOff();


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
    char mapPolishChar(wchar_t c);
    bool _backlight = true;


};
