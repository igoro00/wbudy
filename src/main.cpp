#include <FreeRTOS.h>
#include <task.h>

#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/timer.h>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>

#include <lwip/apps/httpd.h>
#include <lwip/apps/mdns.h>
#include <lwip/init.h>
#include <lwip/ip4_addr.h>

#include "tSound.h"
#include "states.h"

#include "Context.h"
#include "pindefs.hpp"

Context ctx;
QueueHandle_t soundQueue;

// In main.c or a new file, e.g., time_us_32_shim.c
#include "hardware/timer.h"

uint32_t micros32(void) {
    return time_us_32(); // Calls the SDK's static inline function
}

void setupPins(){
	ctx.rgb.init(LED_R, LED_G, LED_B);
	ctx.lcd.init(
		i2c0,
		0x27,
		LCD_SDA,
		LCD_SCL
	);
	ctx.redButton.init(RED_BTN, false, 50);
	ctx.yellowButton.init(YELLOW_BTN, false, 50);
	ctx.resetButton.init(GAME_RST_BTN, false, 50);
	initSound();

	ctx.gameState = GameState::MAIN;
	ctx.taskMutex = xSemaphoreCreateBinary();
	xSemaphoreGive(ctx.taskMutex);
	soundQueue = xQueueCreate(8, sizeof(SoundEffect));
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
	// This function is called if a stack overflow is detected
	// You can add your own error handling code here
	while (1) {
		printf("Stack overflow in task %s\n", pcTaskName);
		printf("Reboot the device\n\n");
		sleep_ms(1000);
	}
}

extern "C" void vApplicationIdleHook() {
    // This function is called when the system is idle
    // You can add your own code here, e.g., low-power mode
    // For now, we just yield to allow other tasks to run
    vTaskDelay(1 / portTICK_PERIOD_MS);
}

extern "C" void vApplicationPassiveIdleHook(void) {
    // Optional: Add low-power code here
}


void main_task(__unused void *params) {
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return;
    }
    // char buff[512];
    while(1){
        // printf("\nTask          State  Prio Stack Num\n");
        // printf("***********************************\n");
        // vTaskList(buff);
        // printf("%s\n", buff);

        // printf("Task            Abs Time    CPU%%\n");
        // printf("***********************************\n");
        // vTaskGetRunTimeStats(buff);
        // printf("%s\n", buff);

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    cyw43_arch_deinit();
}

void tLoop(void *pvParameters) {
	while (1) {
		printf("[Supervisor] trying to get mutex\n");
		if(xSemaphoreTake(ctx.taskMutex, 10000/portTICK_PERIOD_MS) == pdTRUE) {
			printf("[Supervisor] took mutex\n");
			if(ctx.currentTask && eTaskGetState(ctx.currentTask) == eRunning) {
				vTaskDelete(ctx.currentTask);
			}
			ctx.currentTask = NULL;
			switch (ctx.gameState) {
				case GameState::END:
				case GameState::MAIN:
					xTaskCreate(
						sMain,
						"sMain",
						configMINIMAL_STACK_SIZE,
						NULL,
						tskIDLE_PRIORITY + 1,
						&ctx.currentTask
					);
					break;
				case GameState::LOBBY:
					xTaskCreate(
						sLobby,
						"sLobby",
						configMINIMAL_STACK_SIZE * 2,
						NULL,
						tskIDLE_PRIORITY + 1,
						&ctx.currentTask
					);
					break;
				case GameState::GAME:
					xTaskCreate(
						sGame,
						"sGame",
						configMINIMAL_STACK_SIZE * 4,
						NULL,
						tskIDLE_PRIORITY + 2,
						&ctx.currentTask
					);
					break;
			}
			xSemaphoreGive(ctx.taskMutex);
		}
	}
}

int main() {
	stdio_init_all();
	sleep_ms(1000);
	setupPins();

	TaskHandle_t task;

	xTaskCreate(
		main_task,
		"TestMainThread",
		configMINIMAL_STACK_SIZE * 4,
		NULL,
		tskIDLE_PRIORITY,
		&task
	);

	xTaskCreate(
		tLoop,
		"tLoop",
		8192,
		NULL,
		tskIDLE_PRIORITY + 1,
		NULL
	);
	vTaskStartScheduler();

	while (true) {
		printf("Exited Scheduler\n");
		sleep_ms(1000);
	}
}
