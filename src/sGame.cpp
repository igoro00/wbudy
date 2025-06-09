#include "pico/rand.h"
#include <FreeRTOS.h>
#include <hardware/gpio.h>
#include <semphr.h>
#include <stdio.h>
#include <task.h>

#include "Context.h"

#include "pindefs.h"

#include "states.h"
#include "tSound.h"

uint32_t pButtons[2] = {0, 0};
bool canPress = false;

// see: Game.h/Round.player
uint8_t falseStart = 0x0;

uint32_t randint(uint32_t min, uint32_t max) {
	if (min >= max) {
		return min;
	}
	return get_rand_32() % (max - min + 1) + min;
}

void gameButtonISR(uint pin, uint32_t events) {
	if (pin == RED_BTN && pButtons[0] == 0) {
		pButtons[0] = time_us_32();
		falseStart = canPress ? 0 : 1;
	} else if (pin == YELLOW_BTN && pButtons[1] == 0) {
		pButtons[1] = time_us_32();
		falseStart = canPress ? 0 : 2;
	}
}

// false if player 0 is faster
// true if player 1 is faster
// call only if at least one button was pressed
uint8_t fasterPlayer() {
	if (pButtons[0] && pButtons[1]) {
		if (pButtons[0] == pButtons[1]) {
			return 0;
		}
		return (pButtons[0] < pButtons[1]) ? 1 : 2; // lower number == faster
	}
	// only one button pressed
	if (pButtons[0]) {
		return 1; // the other one never pressed
	}
	if (pButtons[1]) {
		return 2; // the other one never pressed
	}
	// no button pressed
	// should never happen
	// because this function is not called
	// when no button is pressed
	return 0;
}

void sGame(void *pvParameters) {
	while (xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) != pdTRUE) {
		printf("[sGame] Waiting for task mutex\n");
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	printf("[sGame] Took task mutex\n");
	Game &game = ctx.nvmem.games[ctx.nvmem.currentGame];
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

	// game starting animation
	{
		ctx.lcd.clear();
		ctx.lcd.setCursor(0, 0);
		ctx.lcd.print("Game is starting in...");
		ctx.lcd.setCursor(1, 0);
		ctx.lcd.print("3 ");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ctx.lcd.print("2 ");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ctx.lcd.print("1 ");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ctx.lcd.print("GO!");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ctx.lcd.clear();
	}

	for (int i = 0; i < sizeof(((Game *)0)->rounds) / sizeof(Round); i++) {
		pButtons[0] = 0;
		pButtons[1] = 0;
		falseStart = 0;
		canPress = false;
		game.rounds[i].player = (get_rand_32() % 2) + 1;
		playSound(SoundEffect::PORTAL2);
		ctx.lcd.clear();
		ctx.lcd.setCursor(0, 0);
		ctx.lcd.printf("Round %d", i + 1);
		ctx.lcd.setCursor(1, 0);
		ctx.lcd.print("Wait for light..");

		vTaskDelay(randint(500, 5000) / portTICK_PERIOD_MS);
		canPress = true;
		playSound(SoundEffect::PRESS);
		ctx.lcd.clear();
		ctx.lcd.setCursor(0, 0);
		ctx.lcd.print("Press the button");
		ctx.rgb.setHSL(
			UIDtoHUE(game.players[game.rounds[i].player - 1]),
			255,
			FotoToL(ctx.fotoValue)
		);
		uint32_t ledStarted = time_us_32();
		while (time_us_32() - ledStarted < 5'000'000) {
			if (!pButtons[0] && !pButtons[1]) {
				continue;
			}
			game.rounds[i].p1_us = pButtons[0] ? pButtons[0] - ledStarted : 0;
			game.rounds[i].p2_us = pButtons[1] ? pButtons[1] - ledStarted : 0;

			// False start means the round is invalid
			if (falseStart) {
				if (falseStart == 1) {
					game.rounds[i].p1_us = UINT32_MAX;
				} else if (falseStart == 2) {
					game.rounds[i].p2_us = UINT32_MAX;
				}
				playSound(SoundEffect::LOST);
				ctx.lcd.clear();
				ctx.lcd.setCursor(0, 0);
				ctx.lcd.print(getPlayerName(game.players[falseStart - 1]));
				ctx.lcd.setCursor(1, 0);
				ctx.lcd.print("false started...");
				break;
			}
			uint8_t faster = fasterPlayer();
			if (faster == 0) {
				playSound(SoundEffect::LOST);
				ctx.lcd.clear();
				ctx.lcd.setCursor(0, 0);
				ctx.lcd.print("TIE!");
				break;
			}
			if (game.rounds[i].player == faster) {
				playSound(SoundEffect::WIN);
				ctx.lcd.clear();
				ctx.lcd.setCursor(0, 0);
				ctx.lcd.printf(
					"%s won!",
					getPlayerName(game.players[faster - 1])
				);
				ctx.lcd.setCursor(1, 0);
				ctx.lcd.printf(
					"%dms",
					(pButtons[faster - 1] - ledStarted) / 1000
				);
				break;
			} else {
				playSound(SoundEffect::LOST);
				ctx.lcd.clear();
				ctx.lcd.setCursor(0, 0);
				ctx.lcd.print(getPlayerName(game.players[faster - 1]));
				ctx.lcd.setCursor(1, 0);
				ctx.lcd.print("lost...");
				break;
			}
		}
		if ((!pButtons[0] && !pButtons[1])) {
			ctx.lcd.clear();
			ctx.lcd.setCursor(0, 0);
			ctx.lcd.print("Timeout!");
		}
		ctx.rgb.setRGB(0);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		ctx.lcd.clear();
	}

	gpio_set_irq_enabled(YELLOW_BTN, GPIO_IRQ_EDGE_FALL, false);
	gpio_set_irq_enabled(RED_BTN, GPIO_IRQ_EDGE_FALL, false);
	ctx.rgb.setRGB(0, 0, 0);
	ctx.nvmem.currentGame++;
	saveGames();
	printf("[sGame] Saved games to flash\n");
	ctx.gameState = GameState::END;
	xSemaphoreGive(ctx.taskMutex);
	printf("[sGame] Gave task mutex\n");
	vTaskDelay(portMAX_DELAY); // waiting for supervisor to delete me
}