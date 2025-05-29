#include "WbudyLCD.h"

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_COMMAND   0
#define LCD_DATA      1

WbudyLCD::WbudyLCD(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t sda, uint8_t scl)
    : _i2c(i2c), _addr(i2c_addr), _sda(sda), _scl(scl) {}


void WbudyLCD::init() {
    set_pin_function_i2c(_sda);
    set_pin_function_i2c(_scl);
    set_pin_pullup(_sda);
    set_pin_pullup(_scl);

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
    sleep_ms_custom(50);

    // Standardowa inicjalizacja HD44780
    sendCmd(0x33); // init
    sendCmd(0x32); // 4-bit
    sendCmd(0x28); // 2 lines, 5x8 font
    sendCmd(0x0C); // display ON, cursor OFF
    sendCmd(0x06); // entry mode

    for (uint8_t i = 0; i < 8; ++i) {
    loadCustomChar(i, polishCharsGraphic[i]);
    }
    
    clear();
}


void WbudyLCD::clear() {
    sendCmd(0x01);
    sleep_ms_custom(2);
}

void WbudyLCD::setCursor(uint8_t row, uint8_t col) {
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    sendCmd(0x80 | (col + row_offsets[row % 4]));
}

void WbudyLCD::sendCmd(uint8_t cmd) {
    sendRegister(cmd, LCD_COMMAND);
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
    uint8_t bl = _backlight ? LCD_BACKLIGHT : 0;
    uint8_t high = (c & 0xF0) | bl | LCD_DATA;
    uint8_t low  = ((c << 4) & 0xF0) | bl | LCD_DATA;
   

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
    uint8_t bl = _backlight ? LCD_BACKLIGHT : 0;
    int8_t high = (data & 0xF0) | bl | mode;
    uint8_t low  = ((data << 4) & 0xF0) | bl | mode;

    writeByteRegister(high);
    toggleEnableRegister(high);
    writeByteRegister(low);
    toggleEnableRegister(low);
}

void WbudyLCD::sleep_ms_custom(uint32_t ms) {
    sleep_ms(ms);
}

void WbudyLCD::set_pin_function_i2c(uint pin) {
    // Funkcja I2C = 3 (FUNCSEL = 3)
    // Każdy pin ma swój rejestr CTRL pod adresem: IO_BANK0_BASE + 0x04 + 8*pin
    volatile uint32_t* ctrl = (volatile uint32_t*)(IO_BANK0_BASE + 0x04 + 8 * pin);
    // Wyczyść bity 0-4 i ustaw na 3
    *ctrl = (*ctrl & ~0x1F) | 3;
}

void WbudyLCD::set_pin_pullup(uint pin) {
    volatile uint32_t* pad = (volatile uint32_t*)(PADS_BANK0_BASE + 0x04 + 0x04 * pin);
    uint32_t mask = (1 << 2) | (1 << 3); // tylko bity PDE i PUE
    uint32_t val = *pad;
    val = (val & ~mask) | (1 << 3);
    *pad = val;
}

void WbudyLCD::loadCustomChar(uint8_t location, const uint8_t charmap[8]) {
    location &= 0x7; // tylko 0-7
    sendRegister(0x40 | (location << 3), LCD_COMMAND); // Ustaw adres CGRAM
    for (int i = 0; i < 8; i++) {
        sendRegister(charmap[i], LCD_DATA);
    }
}

char WbudyLCD::mapPolishChar(wchar_t c) {
    static const wchar_t polishChars[] = {L'ć', L'ę', L'ł', L'ń', L'ó', L'ś', L'ź', L'ż'};
    for (int i = 0; i < 8; ++i) {
        if (c == polishChars[i]) return i;
    }
    return 255; // nie znaleziono
}

void WbudyLCD::printPolish(const wchar_t* str) {
    while (*str) {
        char code = mapPolishChar(*str);
        if (code < 8) {
            sendCharRegister(code); // custom char
        } else if (*str < 128) {
            sendCharRegister((char)*str); // zwykły ASCII
        }
        ++str;
    }
}

void WbudyLCD::backlightOn()  { _backlight = true; }
void WbudyLCD::backlightOff() { _backlight = false; }