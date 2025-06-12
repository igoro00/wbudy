#include "WbudyLCD.h"
#include <cstdio>
#include <stdarg.h>

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0x04
#define LCD_COMMAND 0
#define LCD_DATA 1

WbudyLCD::WbudyLCD(
	i2c_inst_t *i2c,
	uint8_t i2c_addr,
	uint8_t sda,
	uint8_t scl
) {
	init(i2c, i2c_addr, sda, scl);
}

WbudyLCD::WbudyLCD() {}

void WbudyLCD::init(
	i2c_inst_t *i2c,
	uint8_t i2c_addr,
	uint8_t sda,
	uint8_t scl
) {
	this->_i2c = i2c;
	this->_addr = i2c_addr;
	this->_sda = sda;
	this->_scl = scl;

	set_pin_function_i2c(_sda);
	set_pin_function_i2c(_scl);
	set_pin_pullup(_sda);
	set_pin_pullup(_scl);

	i2c_hw_t *hw = _i2c->hw;

	// Wyłącz I2C przed konfiguracją
	hw->enable = 0;

	// Tryb master, standard speed (100kHz), restart enable, slave disable
	hw->con = (1 << 0) |	// master
			  (0b01 << 1) | // speed: standard-mode
			  (1 << 5) |	// restart enable
			  (1 << 6);		// slave disable

	// Adres urządzenia
	hw->tar = _addr;

	// Zegar SCL (dla 100kHz przy 125MHz system clock)
	// TODO: @Adzikoo wziąć system clock ze zmiennej i obliczać
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

void WbudyLCD::sendCmd(uint8_t cmd) { sendData(cmd, LCD_COMMAND); }

void WbudyLCD::sendChar(char c) { sendData(c, LCD_DATA); }

void WbudyLCD::writeByte(uint8_t data) {
	i2c_hw_t *hw = _i2c->hw;
	hw->data_cmd = data;
#undef I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS
#define I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS _u(0x00000010)
	while (!(hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS)) {
		tight_loop_contents();
	}
}

void WbudyLCD::toggleEnable(uint8_t data) {
	sleep_us(1);
	writeByte(data | LCD_ENABLE);
	sleep_us(1);
	writeByte(data & ~LCD_ENABLE);
	sleep_us(100);
}

void WbudyLCD::sendData(uint8_t data, uint8_t mode) {
	uint8_t bl = _backlight ? LCD_BACKLIGHT : 0;
	int8_t high = (data & 0xF0) | bl | mode;
	uint8_t low = ((data << 4) & 0xF0) | bl | mode;

	writeByte(high);
	toggleEnable(high);
	writeByte(low);
	toggleEnable(low);
}

void WbudyLCD::sleep_ms_custom(uint32_t ms) { sleep_ms(ms); }

void WbudyLCD::set_pin_function_i2c(uint pin) {
	// Funkcja I2C = 3 (FUNCSEL = 3)
	// Każdy pin ma swój rejestr CTRL pod adresem: IO_BANK0_BASE + 0x04 + 8*pin
	volatile uint32_t *ctrl =
		reinterpret_cast<volatile uint32_t *>(IO_BANK0_BASE + 0x04 + 8 * pin);
	// Wyczyść bity 0-4 i ustaw na 3
	*ctrl = (*ctrl & ~0x1F) | 3;
}

void WbudyLCD::set_pin_pullup(uint pin) {
	volatile uint32_t *pad = reinterpret_cast<volatile uint32_t *>(
		PADS_BANK0_BASE + 0x04 + 0x04 * pin
	);
	uint32_t mask = (1 << 2) | (1 << 3); // tylko bity PDE i PUE
	uint32_t val = *pad;
	val = (val & ~mask) | (1 << 3);
	*pad = val;
}

void WbudyLCD::loadCustomChar(uint8_t location, const uint8_t charmap[8]) {
	location &= 0x7;				 // tylko 0-7
	sendCmd(0x40 | (location << 3)); // Ustaw adres CGRAM
	for (int i = 0; i < 8; i++) {
		sendChar(charmap[i]);
	}
}

uint8_t WbudyLCD::utf8BytesCounter(uint8_t c) {
	if (!(c & 0b10000000)) {
		return 1; // znak ASCII, 1 bajt
	}
	// sprawdzamy  1x......
	if (!(c & 0b01000000)) {
		return 0; // błąd, znak UTF-8 nie może zaczynać się od 10xxxxxx
	}

	// sprawdzamy  11x.....
	if (!(c & 0b00100000)) {
		// wiemy że 110.....
		return 2;
	}

	// sprawdzamy  111x....
	if (!(c & 0b00010000)) {
		//         1110....
		return 3;
	}

	// sprawdzamy  1111x...
	if (!(c & 0b00001000)) {
		//         11110...
		return 4;
	} else {
		return 0;
	}
}

// https://www.johndcook.com/blog/2019/09/09/how-utf-8-works/
const wchar_t WbudyLCD::walkUTF8String(const uint8_t **pStr) {
	if (pStr == nullptr || *pStr == nullptr)
		return 0xFFFFFFFF;
	uint8_t bytes = utf8BytesCounter(**pStr);
	wchar_t out = 0;
	// bits from header byte
	uint8_t mask = (1 << (8 - bytes - 1)) - 1;
	out += (**pStr & mask)
		   << ((bytes - 1) * 6); // przesuwamy w lewo o 6 bitów na każdy bajt
	for (uint8_t i = 1; i < bytes; i++) {
		(*pStr)++;
		out += ((**pStr) & 0b00111111) << ((bytes - i - 1) * 6);
	}
	return out;
}

uint8_t WbudyLCD::mapPolishChar(const uint8_t **pStr) {
	if (pStr == nullptr || *pStr == nullptr)
		return 0xFE;

	const wchar_t c = walkUTF8String(pStr);
	if (c >= 0xFFFF)
		return 0xFD; // błąd, niepoprawny UTF-8

	for (int i = 0; i < 8; i++) {
		if (c == polishChars[i])
			return i;
	}
	return 0xFF; // nie znaleziono
}

void WbudyLCD::print(const char *str) {
	while (*str) {
		char code = mapPolishChar(reinterpret_cast<const uint8_t **>(&str));
		if (code < 8) {
			sendChar(code); // custom char
		} else if (*str < 128) {
			sendChar(*str); // zwykły ASCII
		}
		str++;
	}
}

void WbudyLCD::printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), fmt, args);

	va_end(args);

	print(buffer);
}

void WbudyLCD::setBacklight(bool val) { _backlight = val; }
