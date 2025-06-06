#include "FreeRTOS.h"

#include "states.h"
#include "Context.h"
#include "stdio.h"
#include "tSound.h"

uint8_t hue = 0;

void goToLobby(WbudyBUTTON *btn) {
    printf("[sMain] Going to lobby\n");\
    playSound(SoundEffect::OK);
    ctx.resetButton.setOnPressed(NULL);
    ctx.gameState = GameState::LOBBY;
    xSemaphoreGive(ctx.taskMutex);
    printf("[sMain] Gave task mutex\n");
}

void sMain(void *pvParameters) {
    while(xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) != pdTRUE) {
        printf("[sMain] Waiting for task mutex\n");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ctx.lcd.clear();
    ctx.lcd.setCursor(0, 0);
    ctx.lcd.print("Loading...");

    loadGames();

    playSound(SoundEffect::MARIO_THEME);
    ctx.resetButton.setOnPressed(goToLobby);
    ctx.lcd.clear();
    ctx.lcd.setCursor(0, 0);
    ctx.lcd.print("Press reset btn");
    ctx.lcd.setCursor(1, 0);
    ctx.lcd.print("to start.");
    while(1) {
        ctx.rgb.setHSL(hue++, 255, 128);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}