#include "WbudyLCD.h"
#include "pico/stdlib.h"

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_COMMAND   0
#define LCD_DATA      1

WbudyLCD::WbudyLCD(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t sda, uint8_t scl)
    : _i2c(i2c), _addr(i2c_addr), _sda(sda), _scl(scl) {}

void WbudyLCD::init() {
    i2c_init(_i2c, 100 * 1000); // 100kHz
    gpio_set_function(_sda, GPIO_FUNC_I2C);
    gpio_set_function(_scl, GPIO_FUNC_I2C);
    gpio_pull_up(_sda);
    gpio_pull_up(_scl);

    sleep_ms(50);

    sendCmd(0x33); // init
    sendCmd(0x32); // 4-bit
    sendCmd(0x28); // 2 lines, 5x8 font
    sendCmd(0x0C); // display ON, cursor OFF
    sendCmd(0x06); // entry mode
    clear();
}

void WbudyLCD::clear() {
    sendCmd(0x01);
    sleep_ms(2);
}

void WbudyLCD::setCursor(uint8_t row, uint8_t col) {
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    sendCmd(0x80 | (col + row_offsets[row % 4]));
}

void WbudyLCD::printChar(char c) {
    sendData((uint8_t)c);
}

void WbudyLCD::print(const char* str) {
    while (*str) printChar(*str++);
}

void WbudyLCD::loadCustomChar(uint8_t location, const uint8_t charmap[8]) {
    location &= 0x7; // tylko 0–7
    sendCmd(0x40 | (location << 3));
    for (int i = 0; i < 8; i++) {
        sendData(charmap[i]);
    }
}

void WbudyLCD::printCustomChar(uint8_t location) {
    sendData(location);
}

void WbudyLCD::sendCmd(uint8_t cmd) {
    send(cmd, LCD_COMMAND);
}

void WbudyLCD::sendData(uint8_t data) {
    send(data, LCD_DATA);
}

void WbudyLCD::send(uint8_t data, uint8_t mode) {
    uint8_t high = (data & 0xF0) | LCD_BACKLIGHT | mode;
    uint8_t low  = ((data << 4) & 0xF0) | LCD_BACKLIGHT | mode;

    writeByte(high);
    toggleEnable(high);
    writeByte(low);
    toggleEnable(low);
}

void WbudyLCD::toggleEnable(uint8_t data) {
    sleep_us(1);
    writeByte(data | LCD_ENABLE);
    sleep_us(1);
    writeByte(data & ~LCD_ENABLE);
    sleep_us(100);
}

void WbudyLCD::writeByte(uint8_t data) {
    i2c_write_blocking(_i2c, _addr, &data, 1, false);
}
