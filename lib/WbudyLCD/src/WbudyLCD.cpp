#include "WbudyLCD.h"
#include "pico/stdlib.h"

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_COMMAND   0
#define LCD_DATA      1

WbudyLCD::WbudyLCD(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t sda, uint8_t scl)
    : _i2c(i2c), _addr(i2c_addr), _sda(sda), _scl(scl) {}

void WbudyLCD::init() {
    gpio_set_function(_sda, GPIO_FUNC_I2C);
    gpio_set_function(_scl, GPIO_FUNC_I2C);
    gpio_pull_up(_sda);
    gpio_pull_up(_scl);

    i2c_hw_t* hw = _i2c->hw;

    // Wyłącz I2C przed konfiguracją
    hw->enable = 0;

    // Tryb master, standard speed (100kHz), restart enable, slave disable
    hw->con =
        (1 << 0) |          // master
        (0b01 << 1) |       // speed: standard-mode
        (1 << 5) |          // restart enable
        (1 << 6);           // slave disable

    // Adres urządzenia
    hw->tar = _addr;

    // Zegar SCL (dla 100kHz przy 125MHz system clock)
    hw->ss_scl_hcnt = 600;
    hw->ss_scl_lcnt = 1300;

    // Włącz I2C
    hw->enable = 1;

    // Czas na ustabilizowanie I2C i LCD
    sleep_ms(50);

    // Standardowa inicjalizacja HD44780
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

void WbudyLCD::sendCmd(uint8_t cmd) {
    sendRegister(cmd, LCD_COMMAND);
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

void WbudyLCD::writeByteRegister(uint8_t data) {
    i2c_hw_t* hw = _i2c->hw;
    hw->data_cmd = data;
    while (!(hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS)) {
        tight_loop_contents();
    }
    // NIE czekaj na STOP tutaj!
}

// Przykład: wyślij jeden znak na LCD przez rejestry
void WbudyLCD::sendCharRegister(char c) {
    uint8_t high = (c & 0xF0) | LCD_BACKLIGHT | LCD_DATA;
    uint8_t low  = ((c << 4) & 0xF0) | LCD_BACKLIGHT | LCD_DATA;

    writeByteRegister(high);
    toggleEnableRegister(high);
    writeByteRegister(low);
    toggleEnableRegister(low);
}

void WbudyLCD::toggleEnableRegister(uint8_t data) {
    sleep_us(1);
    writeByteRegister(data | LCD_ENABLE);
    sleep_us(1);
    writeByteRegister(data & ~LCD_ENABLE);
    sleep_us(100);
}

// Przykład: wyślij tekst na LCD przez rejestry
void WbudyLCD::printRegister(const char* str) {
    while (*str) sendCharRegister(*str++);
}

void WbudyLCD::sendRegister(uint8_t data, uint8_t mode) {
    uint8_t high = (data & 0xF0) | LCD_BACKLIGHT | mode;
    uint8_t low  = ((data << 4) & 0xF0) | LCD_BACKLIGHT | mode;

    writeByteRegister(high);
    toggleEnableRegister(high);
    writeByteRegister(low);
    toggleEnableRegister(low);
}
