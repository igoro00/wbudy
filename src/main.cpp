#include "main.h"

#include "chars.h"
#include "colors.h"
#include "playerData.h"

Context ctx;

void setup() {
	ctx.gameState = GameState::END;
	ctx.cardUID = 0;
	ctx.game = nullptr;
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

	pinMode(YELLOW_BTN, INPUT_PULLUP);
	pinMode(RED_BTN, INPUT_PULLUP);
	pinMode(GAME_RST_BTN, INPUT_PULLUP);
	pinMode(SPEAKER, OUTPUT);

	initFS();
	initWebserver();

	PlaySound(SoundEffect::SETUP_DONE);
	waitForSoundEffect();
	delay(1000);
}


void loop() {
	if (ctx.gameState == GameState::LOBBY) {
		tLobby();
	} else if (ctx.gameState == GameState::GAME) {
		tGame();
	} else {
		bookkeeping();
		if (rtttl::done()) {
			PlaySound(SoundEffect::MAIN_THEME);
		}
		u_int32_t color = HSL2RGB((millis() / 4) % 256, 0xff, 0x7f);
		byte r = color >> 16;
		byte g = color >> 8;
		byte b = color >> 0;

		setLED(r, g, b);
		ctx.lcd->setCursor(0, 0);
		ctx.lcd->print("Game Over");
		if (!digitalRead(GAME_RST_BTN)){
			PlaySound(SoundEffect::OK);
			waitForSoundEffect();
			ctx.gameState = GameState::LOBBY;
		}
	}
}
