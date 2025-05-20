#pragma once
#include <cstdint>
#include "hardware/i2c.h"

class WbudyLCD {
public:
    WbudyLCD(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t sda, uint8_t scl);

    void init();
    void clear();
    void setCursor(uint8_t row, uint8_t col);
    void printChar(char c);
    void print(const char* str);
    void loadCustomChar(uint8_t location, const uint8_t charmap[8]);
    void printCustomChar(uint8_t location);

private:
    i2c_inst_t* _i2c;
    uint8_t _addr;
    uint8_t _sda;
    uint8_t _scl;

    void send(uint8_t data, uint8_t mode);
    void sendCmd(uint8_t cmd);
    void sendData(uint8_t data);
    void toggleEnable(uint8_t data);
    void writeByte(uint8_t data);
};
