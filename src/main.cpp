#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/timer.h>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>

#include <lwip/apps/httpd.h>
#include <lwip/apps/mdns.h>
#include <lwip/init.h>
#include <lwip/ip4_addr.h>

#include <FreeRTOS.h>
#include <task.h>

#include "tSound.h"
#include "states.h"

#include "Context.h"
#include "pindefs.hpp"

Context ctx;
QueueHandle_t soundQueue;

void setupPins(){
	ctx.gameState = GameState::MAIN;
	ctx.taskMutex = xSemaphoreCreateMutex();
	soundQueue = xQueueCreate(8, sizeof(SoundEffect));
	initSound();

	ctx.redButton.init(RED_BTN, false, 50);
	ctx.yellowButton.init(YELLOW_BTN, false, 50);
	ctx.rgb.init(LED_R, LED_G, LED_B);
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

void main_task(__unused void *params) {
	if (cyw43_arch_init()) {
		printf("failed to initialise\n");
		return;
	}
	while(1){
		printf("REDBTN = %d", ctx.redButton.isPressed());
		printf(" debounced = %d", ctx.redButton.debounced);
		printf(" gpio_get = %d\n", gpio_get(RED_BTN));
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	cyw43_arch_deinit();
}

void tLoop(void *pvParameters) {
	while (1) {
		printf("[Supervisor] trying to get mutex\n");
		if(xSemaphoreTake(ctx.taskMutex, 10000/portTICK_PERIOD_MS) == pdTRUE) {
			printf("[Supervisor] took mutex");
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
	sleep_ms(10000);
	setupPins();

	TaskHandle_t task;

	xTaskCreate(
		main_task,
		"TestMainThread",
		configMINIMAL_STACK_SIZE,
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
