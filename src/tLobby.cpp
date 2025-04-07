#include "LiquidCrystal_I2C.h"

#include "main.hpp"
#include "chars.hpp"
#include "sysutils.hpp"
#include "pindefs.hpp"
#include "sound.hpp"
#include "colors.hpp"

bool updateLCD;

// settingPlayer:
// 0 - no one
// 1 - player 1
// 4 - player 2
void updateLobbyLCD(byte settingPlayer = 0) {
	updateLCD = false;
	LiquidCrystal_I2C *lcd = ctx.lcd;
	lcd->clear();

	lcd->setCursor(0, 0);
	if (settingPlayer == 1) {
		lcd->print("_");
	}
	if (ctx.game->getPlayer(0).uid == 0) {
		lcd->print("Player 1");
	} else {
		printPolishMsg(*ctx.lcd, ctx.game->getPlayer(0).name);
	}
	if (settingPlayer == 1) {
		lcd->print("_");
	}

	lcd->setCursor(0, 1);
	if (settingPlayer == 2) {
		lcd->print("_");
	}
	if (ctx.game->getPlayer(1).uid == 0) {
		lcd->print("Player 2");
	} else {
		printPolishMsg(*ctx.lcd, ctx.game->getPlayer(1).name);
	}
	if (settingPlayer == 2) {
		lcd->print("_");
	}
}

void lobbyButtonISR(uint pin, uint32_t events) {
	updateLCD = true;
	if (events & GPIO_IRQ_EDGE_FALL) {
		PlaySound(SoundEffect::OK);
	}
}

#define START_GAME_TIME 1500
void tLobby() {
	updateLCD = true;
	gpio_set_irq_enabled_with_callback(
		YELLOW_BTN,
		GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
		true,
		&lobbyButtonISR
	);
	gpio_set_irq_enabled_with_callback(
		RED_BTN,
		GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
		true,
		&lobbyButtonISR
	);
	if (ctx.game == nullptr) {
		ctx.game = std::make_unique<Game>(0, 0);
	} else {
		// we dont pass Player
		// so it has time to refresh from the file system
		ctx.game = std::make_unique<Game>(
			ctx.game->getPlayer(0).uid,
			ctx.game->getPlayer(1).uid
		);
	}

	uint32_t lastRST = 0;
	while (1) {
		bookkeeping();
		bool yellowBtn = !digitalRead(YELLOW_BTN);
		bool redBtn = !digitalRead(RED_BTN);
		bool rstBtn = !digitalRead(GAME_RST_BTN);

		if (updateLCD)
			updateLobbyLCD(redBtn | yellowBtn << 1);

		if (rstBtn && ctx.game->getPlayer(0).uid && ctx.game->getPlayer(1).uid &&
			!redBtn && !yellowBtn) {
			if (lastRST == 0) {
				lastRST = millis();
			} else if (millis() - lastRST > START_GAME_TIME) {
				ctx.gameState = GameState::GAME;
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
			continue;
		}

		// At this point exactly one button is pressed

		uint32_t uid = tRfidRead();
		if (uid) {
			setLEDByUID(uid);
			ctx.game->setPlayer(yellowBtn, uid);
			updateLCD = true;
			wait(100);
		}
	}
	gpio_set_irq_enabled(
		RED_BTN,
		GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
		false
	);
	gpio_set_irq_enabled(
		YELLOW_BTN,
		GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
		false
	);
}