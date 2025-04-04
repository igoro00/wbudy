#include "main.h"

#include "chars.h"
#include "colors.h"
#include "pitches.h"
#include "playerData.h"
#include <NonBlockingRtttl.h>

Context ctx;

void setup() {
	ctx.gameState = GameState::END;
	ctx.cardUID = 0;
	ctx.players[0] = 0;
	ctx.players[1] = 0;
	LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27, 16, 2);
	ctx.lcd = lcd;
	MFRC522 *rfid = new MFRC522(RFID_CS, RFID_RST);
	ctx.rfid = rfid;
	Serial.begin(115200);
	SPI.setRX(RFID_MISO);
	SPI.setTX(RFID_MOSI);
	SPI.setSCK(RFID_SCK);
	SPI.setCS(RFID_CS);
	SPI.begin();
	Wire.begin();
	lcd->init();
	loadChars(lcd);
	lcd->backlight();

	rfid->PCD_Init();
	rfid->PCD_WriteRegister(rfid->ComIEnReg, 0xA0);

	// pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LED_R, OUTPUT);
	pinMode(LED_G, OUTPUT);
	pinMode(LED_B, OUTPUT);

	// digitalWrite(LED_BUILTIN, 1);
	analogWrite(LED_R, 255);
	analogWrite(LED_G, 255);
	analogWrite(LED_B, 255);
	digitalWrite(SPEAKER, 0);

	pinMode(YELLOW_BTN, INPUT_PULLUP);
	pinMode(RED_BTN, INPUT_PULLUP);
	pinMode(GAME_RST_BTN, INPUT_PULLUP);
	pinMode(SPEAKER, OUTPUT);
	digitalWrite(SPEAKER, 0);
	initFS();
	initWebserver();

	tone(SPEAKER, NOTE_A4, 100);
	delay(100);
	tone(SPEAKER, NOTE_B4, 100);
	delay(100);
}
void loop() {
	if (ctx.gameState == GameState::LOBBY) {
		tLobby(ctx);
	} else if (ctx.gameState == GameState::GAME) {
		tGame(ctx);
	} else {
		bookkeeping();
		if (rtttl::done()) {
			rp2040.fifo.push((uint32_t)SoundEffectBuffer(SoundEffect::MAIN_THEME));
		}

		u_int32_t color = HSL2RGB((millis() / 4) % 256, 0xff, 0x7f);
		byte r = color >> 16;
		byte g = color >> 8;
		byte b = color >> 0;

		setLED(r, g, b);
	}
}

// second core only for sound
void setup1() {
	pinMode(SPEAKER, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(SPEAKER, 0);
}
void loop1() {
	if (rp2040.fifo.available()) {

		// pop returns uint32_t
		// but since were using 32bit cpu
		// we can cast it to pointer
		const char *soundEffect = (const char *)rp2040.fifo.pop();
		// the c string should contain the RTTTL song
		// it will overwrite the currently playing song
		rtttl::begin(SPEAKER, soundEffect);
	}
	if (rtttl::isPlaying()) {
		rtttl::play(); // next tick
	}
}