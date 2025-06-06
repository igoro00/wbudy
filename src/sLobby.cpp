#include <hardware/gpio.h>
#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "pindefs.h"
#include "Context.h"

#include "states.h"

#include "tSound.h"

bool updateLCD;

// settingPlayer:
// 0 - no one
// 1 - player 1
// 4 - player 2
void updateLobbyLCD(uint8_t settingPlayer = 0) {
	// updateLCD = false;
	// LiquidCrystal_I2C *lcd = ctx.lcd;
	// lcd->clear();

	// lcd->setCursor(0, 0);
	// if (settingPlayer == 1) {
	// 	lcd->print("_");
	// }
	// if (ctx.game->getPlayer(0).uid == 0) {
	// 	lcd->print("Player 1");
	// } else {
	// 	printPolishMsg(*ctx.lcd, ctx.game->getPlayer(0).name);
	// }
	// if (settingPlayer == 1) {
	// 	lcd->print("_");
	// }

	// lcd->setCursor(0, 1);
	// if (settingPlayer == 2) {
	// 	lcd->print("_");
	// }
	// if (ctx.game->getPlayer(1).uid == 0) {
	// 	lcd->print("Player 2");
	// } else {
	// 	printPolishMsg(*ctx.lcd, ctx.game->getPlayer(1).name);
	// }
	// if (settingPlayer == 2) {
	// 	lcd->print("_");
	// }
}

void btnPressed(WbudyBUTTON *btn) {
	printf("[sLobby] Button pressed on pin %d\n", btn->getPin());
	playSound(SoundEffect::OK);
}

void goToGame(WbudyBUTTON *btn) {
	printf("[sLobby] Going to game\n");
	ctx.redButton.setOnPressed(NULL);
	ctx.yellowButton.setOnPressed(NULL);
	ctx.resetButton.setOnPressed(NULL);
	ctx.rgb.setRGB(0, 0, 0);
	ctx.gameState = GameState::GAME;
	xSemaphoreGive(ctx.taskMutex);
	printf("[sLobby] Gave task mutex\n");
}

#define START_GAME_TIME 1000
void sLobby(void *pvParameters) {
	while(xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) != pdTRUE) {
		printf("[sLobby] Waiting for task mutex\n");
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	while(ctx.resetButton.isPressed()) {
		printf("[sLobby] Waiting for reset button to be released\n");
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	printf("[sLobby] Took task mutex\n");
	ctx.redButton.setOnPressed(btnPressed);
	ctx.resetButton.setOnLongPressed(goToGame);
	ctx.lcd.clear();
	ctx.lcd.setCursor(0, 0);
	ctx.lcd.print("Waiting for cards");
	// if (ctx.game == nullptr) {
	// 	ctx.game = std::make_unique<Game>(0, 0);
	// } else {
	// 	// we dont pass Player
	// 	// so it has time to refresh from the file system
	// 	ctx.game = std::make_unique<Game>(
	// 		ctx.game->getPlayer(0).uid,
	// 		ctx.game->getPlayer(1).uid
	// 	);
	// }

	// uint32_t lastRST = 0;
	while (1) {
		if (ctx.resetButton.isPressed()) {

			float x = (float)ctx.resetButton.msSinceChange() / START_GAME_TIME;
			static const float k = 4.0;
			float val = 255.0 * (exp(k * x) - 1.0) / (exp(k) - 1.0);
			ctx.rgb.setRGB((uint8_t)val,0,0);
		} else {
			ctx.rgb.setRGB(0, 0, 0);
		}

	// 	if (rfidOn && !rfidShouldBe) {
	// 		rfidOn = false;
	// 		ctx.rfid->PCD_SoftPowerDown();
	// 	} else if (!rfidOn && rfidShouldBe) {
	// 		rfidOn = true;
	// 		ctx.rfid->PCD_Init();
	// 	}
	// 	digitalWrite(LED_BUILTIN, rfidOn);

	// 	rfidShouldBe = false;
	// 	bool yellowBtn = !digitalRead(YELLOW_BTN);
	// 	bool redBtn = !digitalRead(RED_BTN);
	// 	bool rstBtn = !digitalRead(GAME_RST_BTN);

	// 	if (updateLCD)
	// 		updateLobbyLCD(redBtn | yellowBtn << 1);

	// 	if (rstBtn && ctx.game->getPlayer(0).uid && ctx.game->getPlayer(1).uid &&
	// 		!redBtn && !yellowBtn) {
	// 		if (lastRST == 0) {
	// 			lastRST = millis();
	// 		} else if (millis() - lastRST > START_GAME_TIME) {
	// 			ctx.gameState = GameState::GAME;
	// 			setLED(0, 0, 0);
	// 			for (int i = 0; i < 5; i++) {
	// 				setLED(255, 0, 0);
	// 				delay(50);
	// 				setLED(0, 0, 0);
	// 				delay(50);
	// 			}
	// 			break;
	// 		} else {
	// 			byte r = pow(map(millis() - lastRST, 0, START_GAME_TIME, 0, 63), 3)/1024;
	// 			setLED(r, 0, 0);
	// 		}
	// 	} else {
	// 		if (lastRST) {
	// 			lastRST = 0;
	// 			setLED(0, 0, 0);
	// 		}
	// 	}

	// 	if (yellowBtn && redBtn) {
	// 		// Serial.println("Both buttons pressed");
	// 		// not allowed
	// 		continue;
	// 	}

	// 	// cancel RFID task if both buttons are not pressed
	// 	if (!yellowBtn && !redBtn) {
	// 		if (!rstBtn) {
	// 			setLED(0, 0, 0);
	// 		}
	// 		continue;
	// 	}

	// 	// At this point exactly one button is pressed

	// 	rfidShouldBe = true;
	// 	if(rfidOn) {
	// 		uint32_t uid = tRfidRead();
	// 		if (uid) {
	// 			setLEDByUID(uid);
	// 			ctx.game->setPlayer(yellowBtn, uid);
	// 			updateLCD = true;
	// 			wait(100);
	// 		}
	// 	}
		vTaskDelay(100 / portTICK_PERIOD_MS); // should be replace by a semaphore or something
	}
}