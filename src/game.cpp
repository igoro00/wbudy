#include "main.h"

// uint32_t pButtons[2] = {0, 0};

// void playerButtonISR(uint pin, uint32_t events) {
// 	if (events & GPIO_IRQ_EDGE_RISE) {
// 		return;
// 	}
// 	if (pin == YELLOW_BTN) {
// 		pButtons[0] = micros();
// 	} else if (pin == RED_BTN) {
// 		pButtons[1] = micros();
// 	}
// 	rp2040.fifo.push(SoundEffectBuffer(SoundEffect::OK));
// }

void tGame() {
	if(ctx.game == nullptr) {
		Serial.println("GAME IS NULL");
		ctx.gameState = GameState::END;
	}
	LiquidCrystal_I2C *lcd = ctx.lcd;
	lcd->clear();
	lcd->setCursor(0, 0);
	lcd->print("Game is starting in...");
	lcd->setCursor(0, 1);
	lcd->print("3 ");
	delay(1000);
	lcd->print("2 ");
	delay(1000);
	lcd->print("1 ");
	delay(1000);
	lcd->print("GO!");
	delay(1000);
	lcd->clear();
	randomSeed(millis());

	for (int i = 0; i < 5; i++) {
		long player = random(2);
		PlaySound(SoundEffect::STOP);
		PlaySound(SoundEffect::GAME_WAITING);
		PlaySound(SoundEffect::LOOP_LAST);
		lcd->clear();
		lcd->setCursor(0, 0);
		lcd->print("Wait for light..");
		delay(random(5000, 50000));
		PlaySound(SoundEffect::STOP);
		PlaySound(SoundEffect::PRESS);
		lcd->clear();
		lcd->setCursor(0, 0);
		lcd->print("Press the button");
		setLEDByUID(ctx.game->getPlayer(player));
		uint32_t ledStarted = millis();
		bool clicked = false;
		while (millis() - ledStarted < 5000) {
			bookkeeping();
			bool p1Btn = !digitalRead(RED_BTN);
			bool p2Btn = !digitalRead(YELLOW_BTN);
			if (!p1Btn && !p2Btn) {
				continue;
			}
			if (p1Btn && p2Btn) {
				lcd->clear();
				lcd->setCursor(0, 0);
				lcd->print("Both players");
				lcd->setCursor(0, 1);
				lcd->print("pressed!");
				clicked = true;
				break;
			}
			if ((player == 0 && p1Btn) || (player == 1 && p2Btn)) {
				PlaySound(SoundEffect::WIN);
				lcd->clear();
				lcd->setCursor(0, 0);
				lcd->print("Player ");
				lcd->print(player + 1);
				lcd->print(" wins!");
				lcd->setCursor(0, 1);
				lcd->print(millis() - ledStarted);
				lcd->print("ms");
				clicked = true;
				break;
			} else {
				lcd->clear();
				lcd->setCursor(0, 0);
				lcd->print("Player ");
				lcd->print(player + 1);
				lcd->print(" lost!");
				clicked = true;
				break;
			}
		}
		if (!clicked) {
			lcd->clear();
			lcd->setCursor(0, 0);
			lcd->print("Timeout!");
		}
		setLED(0, 0, 0);
		delay(2000);
		lcd->clear();
	}
	ctx.gameState = GameState::END;
}