#include "LiquidCrystal_I2C.h"

#include "main.hpp"
#include "pindefs.hpp"
#include "sound.hpp"
#include "chars.hpp"
#include "sysutils.hpp"
#include "colors.hpp"

uint32_t pButtons[2] = {0, 0};

void gameButtonISR(uint pin, uint32_t events) {
	if (pin == YELLOW_BTN && pButtons[0] == 0) {
		pButtons[0] = micros();
	} else if (pin == RED_BTN && pButtons[1] == 0) {
		pButtons[1] = micros();
	}
}

// false if player 0 is faster
// true if player 1 is faster
// call only if at least one button was pressed
bool fasterPlayer(){
	if(pButtons[0] && pButtons[1]){
		return pButtons[0] >= pButtons[1];
	}
	// only one button pressed
	if(pButtons[0]){
		return 0; // the other one never pressed
	}
	if(pButtons[1]){
		return 1; // the other one never pressed
	}
	return false; // should never happen
}

void tGame() {
	if (ctx.game == nullptr) {
		Serial.println("GAME IS NULL");
		ctx.gameState = GameState::END;
	}
	gpio_set_irq_enabled_with_callback(
		YELLOW_BTN,
		GPIO_IRQ_EDGE_FALL,
		true,
		&gameButtonISR
	);
	gpio_set_irq_enabled_with_callback(
		RED_BTN,
		GPIO_IRQ_EDGE_FALL,
		true,
		&gameButtonISR
	);
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
		pButtons[0] = 0;
		pButtons[1] = 0;
		long player = random(2);
		PlaySound(SoundEffect::STOP);
		PlaySound(SoundEffect::GAME_WAITING);
		PlaySound(SoundEffect::LOOP_LAST);
		lcd->clear();
		lcd->setCursor(0, 0);
		lcd->print("Wait for light..");
		delay(random(500, 5000));
		PlaySound(SoundEffect::STOP);
		PlaySound(SoundEffect::PRESS);
		lcd->clear();
		lcd->setCursor(0, 0);
		lcd->print("Press the button");
		setLEDByUID(ctx.game->getPlayer(player));
		uint32_t ledStarted = micros();
		while (micros() - ledStarted < 5'000'000) {
			bookkeeping();
			if (!pButtons[0] && !pButtons[1]) {
				continue;
			}
			bool faster = fasterPlayer();
			ctx.game->addRound(
				pButtons[0]-ledStarted,
				pButtons[1]-ledStarted
			);
			if (player == faster) {
				PlaySound(SoundEffect::WIN);
				lcd->clear();
				lcd->setCursor(0, 0);
				printPolishMsg(*lcd, ctx.game->getPlayerName(player));
				lcd->print(" won!");
				lcd->setCursor(0, 1);
				lcd->print((pButtons[player]-ledStarted)/1000);
				lcd->print("ms");
				break;
			} else {
				PlaySound(SoundEffect::LOST);
				lcd->clear();
				lcd->setCursor(0, 0);
				printPolishMsg(*lcd, ctx.game->getPlayerName(player));
				lcd->setCursor(0, 1);
				lcd->print("lost...");
				break;
			}
		}
		if ((!pButtons[0] && !pButtons[1])) {
			lcd->clear();
			lcd->setCursor(0, 0);
			lcd->print("Timeout!");
		}
		setLED(0, 0, 0);
		delay(2000);
		lcd->clear();
	}
	gpio_set_irq_enabled(
		YELLOW_BTN,
		GPIO_IRQ_EDGE_FALL,
		false
	);
	gpio_set_irq_enabled(
		RED_BTN,
		GPIO_IRQ_EDGE_FALL,
		false
	);
	
	ctx.game->toJSON();
	ctx.gameState = GameState::END;
}