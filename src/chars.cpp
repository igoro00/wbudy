#include "main.h"
#include "chars.h"

// znaki i ich grafiki muszą być w tej samej kolejności

static const wchar_t polishChars[] = {L'ć', L'ę', L'ł', L'ń',
	L'ó', L'ś', L'ź', L'ż'};

uint8_t polishCharsGraphic[8][8]{
{0x02, 0x04, 0x00, 0x0E, 0x10, 0x10, 0x11, 0x0E}, // ć
{0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x04, 0x02}, // ę
{0x0C, 0x04, 0x04, 0x06, 0x0C, 0x04, 0x0E, 0x00}, // ł
{0x02, 0x04, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11}, // ń
{0x02, 0x04, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E}, // ó
{0x02, 0x04, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x1E}, // ś
{0x02, 0x04, 0x00, 0x1F, 0x02, 0x04, 0x08, 0x1F}, // ź
{0x00, 0x04, 0x00, 0x1F, 0x02, 0x04, 0x08, 0x1F}  // ż
};

void loadChars(LiquidCrystal_I2C *lcd) {
	for (uint8_t i = 0; i < 8; i++) {
		lcd->createChar(i, polishCharsGraphic[i]);
	}
}

void printPolishChar(LiquidCrystal_I2C &lcd, const wchar_t ch) {
	int index = -1; // Default index if not found

	// Iterate over the regular array to find the character
	for (int i = 0; i < 8; i++) {
		if (ch == polishChars[i]) {
			index = i;
			break;
		}
	}

	if (index != -1) {
		lcd.write(index);
	} else if (ch < 128) { // ASCII range
		lcd.print(static_cast<char>(ch));
	} else {
		lcd.print('?');
	}
}

void printPolishMsg(LiquidCrystal_I2C &lcd, const wchar_t *msg){
	for (const wchar_t *ch = msg; *ch != L'\0'; ch++) {
		printPolishChar(lcd, *ch);
	}
}

// void printPolishMsg(LiquidCrystal_I2C &lcd, const std::wstring &msg) {
// 	for (wchar_t ch : msg) {
// 		printPolishChar(lcd, ch);
// 	}
// }