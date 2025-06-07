#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <stdio.h>
#include <hardware/gpio.h>
#include "pico/rand.h"

#include "Context.h"

#include "pindefs.h"

#include "states.h"
#include "tSound.h"

uint32_t pButtons[2] = {0, 0};
bool canPress = false;

// see: Game.h/Round.player
uint8_t falseStart = 0;

uint8_t UIDtoHUE(uint32_t input) {
	uint8_t hash = 0;

	// Process each byte of the input
	for (int i = 0; i < 4; ++i) {
		hash ^= (input >> (i * 8)) & 0xFF; // XOR each byte into hash
		hash = (hash << 3) | (hash >> 5);  // Rotate left by 3 bits
	}

	return hash;
}

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
	// no button pressed
	// should never happen 
	// because this function is not called 
	// when no button is pressed
	return false; 
}

void sGame(void *pvParameters) {
	while(xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) != pdTRUE) {
        printf("[sGame] Waiting for task mutex\n");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
	printf("[sGame] Took task mutex\n");
	Game* game = getNewGame();
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
	
	for (int i = 0; i < 2; i++) {
		pButtons[0] = 0;
		pButtons[1] = 0;
		falseStart = 0;
		canPress = false;
		long player = get_rand_32()%2;
		playSound(SoundEffect::PORTAL2);
		ctx.lcd.clear();
		ctx.lcd.setCursor(0, 0);
		ctx.lcd.print("Wait for light..");

		 
		vTaskDelay(randint(500, 5000) / portTICK_PERIOD_MS);
		canPress = true;
		playSound(SoundEffect::PRESS);
		ctx.lcd.clear();
		ctx.lcd.setCursor(0, 0);
		ctx.lcd.print("Press the button");
		ctx.rgb.setHSL(
			0,
			255,
			128
		);
		uint32_t ledStarted = time_us_32();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	// 	while (micros() - ledStarted < 5'000'000) {
	// 		bookkeeping();
	// 		if (!pButtons[0] && !pButtons[1]) {
	// 			continue;
	// 		}

	// 		// False start means the round is invalid
	// 		if (falseStart) {
	// 			playSound(SoundEffect::LOST);
	// 			ctx.lcd.clear();
	// 			ctx.lcd.setCursor(0, 0);
	// 			printPolishMsg(*lcd, ctx.game->getPlayer(falseStart-1).name);
	// 			ctx.lcd.setCursor(1, 0);
	// 			ctx.lcd.print("false started...");
	// 			break;
	// 		}
	// 		bool faster = fasterPlayer();
	// 		ctx.game->addRound(
	// 			player==0,
	// 			pButtons[0] ? pButtons[0]-ledStarted+1 : 0,
	// 			pButtons[1] ? pButtons[1]-ledStarted+1 : 0
	// 		);
	// 		if (player == faster) {
	// 			playSound(SoundEffect::WIN);
	// 			ctx.lcd.clear();
	// 			ctx.lcd.setCursor(0, 0);
	// 			printPolishMsg(*lcd, ctx.game->getPlayer(player).name);
	// 			ctx.lcd.print(" won!");
	// 			ctx.lcd.setCursor(1, 0);
	// 			ctx.lcd.print((pButtons[player]-ledStarted)/1000);
	// 			ctx.lcd.print("ms");
	// 			break;
	// 		} else {
	// 			playSound(SoundEffect::LOST);
	// 			ctx.lcd.clear();
	// 			ctx.lcd.setCursor(0, 0);
	// 			printPolishMsg(*lcd, ctx.game->getPlayer(faster).name);
	// 			ctx.lcd.setCursor(1, 0);
	// 			ctx.lcd.print("lost...");
	// 			break;
	// 		}
	// 	}
	// 	if ((!pButtons[0] && !pButtons[1])) {
	// 		ctx.lcd.clear();
	// 		ctx.lcd.setCursor(0, 0);
	// 		ctx.lcd.print("Timeout!");
	// 	}
		ctx.rgb.setRGB(0);
	// 	delay(2000);
		ctx.lcd.clear();
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
	ctx.rgb.setRGB(0, 0, 0);
	saveGames();
	printf("[sGame] Saved games to flash\n");
	ctx.gameState = GameState::END;
	xSemaphoreGive(ctx.taskMutex);
	printf("[sGame] Gave task mutex\n");
	vTaskDelay(portMAX_DELAY); // waiting for supervisor to delete me
}