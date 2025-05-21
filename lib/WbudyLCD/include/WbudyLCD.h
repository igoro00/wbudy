#pragma once
#include <cstdint>
#include "hardware/i2c.h"

class WbudyLCD {
public:
    WbudyLCD(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t sda, uint8_t scl);

    void init();
    void clear();
    void setCursor(uint8_t row, uint8_t col);
    void printRegister(const char* str);
    void printCharRegister(char c);

private:
    i2c_inst_t* _i2c;
    uint8_t _addr;
    uint8_t _sda;
    uint8_t _scl;

    void send(uint8_t data, uint8_t mode);
    void sendRegister(uint8_t data, uint8_t mode);
    void sendCmd(uint8_t cmd);
    void toggleEnable(uint8_t data);
    void writeByteRegister(uint8_t data);
    void sendCharRegister(char c);
    void sendCmdRegister(uint8_t cmd);
    void sendDataRegister(uint8_t data);
    void toggleEnableRegister(uint8_t data);
    
};
