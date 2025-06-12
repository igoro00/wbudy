#include <hardware/gpio.h>
#include <math.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "Context.h"
#include "pindefs.h"

#include "states.h"

#include "tSound.h"

// settingPlayer:
// 0 - no one
// 1 - player 1
// 4 - player 2
void updateLobbyLCD() {
	ctx.lcd.clear();

	ctx.lcd.setCursor(0, 0);
	if (ctx.redButton.isPressed()) {
		ctx.lcd.print("_");
	}
	if (ctx.nvmem.games[ctx.nvmem.currentGame].players[0] == 0) {
		ctx.lcd.print("Player 1");
	} else {
		const char *playerName =
			getPlayerName(ctx.nvmem.games[ctx.nvmem.currentGame].players[0]);
		if (playerName == NULL) {
			ctx.lcd.printf("0x%08X",
				ctx.nvmem.games[ctx.nvmem.currentGame].players[0]);
		} else {
			ctx.lcd.print(playerName);
		}
	}
	if (ctx.redButton.isPressed()) {
		ctx.lcd.print("_");
	}

	ctx.lcd.setCursor(1, 0);
	if (ctx.yellowButton.isPressed()) {
		ctx.lcd.print("_");
	}
	if (ctx.nvmem.games[ctx.nvmem.currentGame].players[1] == 0) {
		ctx.lcd.print("Player 2");
	} else {
		const char *playerName =
			getPlayerName(ctx.nvmem.games[ctx.nvmem.currentGame].players[1]);
		if (playerName == NULL) {
			ctx.lcd.printf("0x%08X",
				ctx.nvmem.games[ctx.nvmem.currentGame].players[1]);
		} else {
			ctx.lcd.print(playerName);
		}
	}
	if (ctx.yellowButton.isPressed()) {
		ctx.lcd.print("_");
	}
}

void btnPressed(WbudyBUTTON *btn) {
	printf("[sLobby] Button pressed on pin %d\n", btn->getPin());
	playSound(SoundEffect::OK);
	updateLobbyLCD();
}

void goToGame(WbudyBUTTON *btn) {
	printf("[sLobby] Going to game\n");
	ctx.redButton.clearCallbacks();
	ctx.yellowButton.clearCallbacks();
	ctx.resetButton.clearCallbacks();
	ctx.lcd.clear();
	ctx.rgb.setRGB(0, 0, 0);
	ctx.gameState = GameState::GAME;
	xSemaphoreGive(ctx.taskMutex);
	printf("[sLobby] Gave task mutex\n");
}

void onCardScanned(uint32_t uid) {
	if (ctx.redButton.isPressed()) {
		ctx.nvmem.games[ctx.nvmem.currentGame].players[0] = uid;
	}
	if (ctx.yellowButton.isPressed()) {
		ctx.nvmem.games[ctx.nvmem.currentGame].players[1] = uid;
	}
	updateLobbyLCD();
	playSound(SoundEffect::OK);
}

#define START_GAME_TIME 1000
void sLobby(void *pvParameters) {
	while (xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) != pdTRUE) {
		printf("[sLobby] Waiting for task mutex\n");
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	printf("[sLobby] Took task mutex\n");
	while (ctx.resetButton.isPressed()) {
		printf("[sLobby] Waiting for reset button to be released\n");
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	ctx.redButton.setOnPressed(btnPressed);
	ctx.yellowButton.setOnPressed(btnPressed);

	ctx.redButton.setOnChanged([](WbudyBUTTON *btn, bool state) {
		updateLobbyLCD();
	});
	ctx.yellowButton.setOnChanged([](WbudyBUTTON *btn, bool state) {
		updateLobbyLCD();
	});

	ctx.resetButton.setOnLongPressed(goToGame);

	updateLobbyLCD();
	while (1) {
		if (ctx.resetButton.isPressed()) {

			// funkcja wykładnicza przechodząca przez (0,0) i (255,255)
			float x = (float)ctx.resetButton.msSinceChange() / START_GAME_TIME;
			static const float k = 4.0;
			float val = 255.0 * (exp(k * x) - 1.0) / (exp(k) - 1.0);

			ctx.rgb.setRGB((uint8_t)val, 0, 0);
		} else {
			if (ctx.redButton.isPressed() || ctx.yellowButton.isPressed()) {
				ctx.rfid.setOnScanned(onCardScanned);
				ctx.rgb.setHSL(
					UIDtoHUE(ctx.nvmem.games[ctx.nvmem.currentGame]
								 .players[ctx.yellowButton.isPressed()]),
					255,
					FotoToL(ctx.fotoValue)
				);
			} else {
				ctx.rfid.setOnScanned(NULL);
				ctx.rgb.setRGB(0, 0, 0);
			}
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}