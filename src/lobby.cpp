#include "main.h"

void updateLobbyLCD(Context &ctx) {
	LiquidCrystal_I2C *lcd = ctx.lcd;
	lcd->clear();
	lcd->setCursor(0, 0);
	if (ctx.players[0] == 0) {
		lcd->print("Player 1");
	} else {
		lcd->print(ctx.players[0], HEX);
	}
	lcd->setCursor(0, 1);
	if (ctx.players[1] == 0) {
		lcd->print("Player 2");
	} else {
		lcd->print(ctx.players[1], HEX);
	}
}

#define START_GAME_TIME 1500
void tLobby(Context &ctx) {
	updateLobbyLCD(ctx);
	uint32_t lastRST = 0;
	while (1) {
		bookkeeping();
		// Serial.println("Lobby loop");
		bool yellowBtn = !digitalRead(YELLOW_BTN);
		bool redBtn = !digitalRead(RED_BTN);
		bool rstBtn = !digitalRead(GAME_RST_BTN);
		if (rstBtn && ctx.players[0] && ctx.players[1] && !redBtn &&
			!yellowBtn) {
			if (lastRST == 0) {
				lastRST = millis();
			} else if (millis() - lastRST > START_GAME_TIME) {
				ctx.gameState = GameState::GAME;
				// Serial.println("Game started");
				setLED(0, 0, 0);
				for (int i = 0; i < 5; i++) {
					setLED(255, 0, 0);
					delay(50);
					setLED(0, 0, 0);
					delay(50);
				}
				break;
			} else {
				byte r = map(millis() - lastRST, 0, START_GAME_TIME, 0, 200);
				setLED(r, 0, 0);
			}
		} else {
			if (lastRST) {
				lastRST = 0;
				setLED(0, 0, 0);
			}
		}

		if (yellowBtn && redBtn) {
			// Serial.println("Both buttons pressed");
			// not allowed
			continue;
		}

		// cancel RFID task if both buttons are not pressed
		if (!yellowBtn && !redBtn) {
			if (!rstBtn) {
				setLED(0, 0, 0);
			}
			// Serial.println("No buttons pressed");
			continue;
		}
		// Serial.println("One button pressed");

		// At this point exactly one button is pressed

		uint32_t uid = tRfidRead(ctx);
		if (uid) {
			Serial.print("UID: ");
			Serial.println(uid, HEX);
			setLEDByUID(uid);
			if (redBtn) {
				ctx.players[0] = uid;
			} else {
				ctx.players[1] = uid;
			}
			updateLobbyLCD(ctx);
			wait(100);
		} else {
			Serial.println("No UID");
		}
	}
}