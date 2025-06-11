#include <FreeRTOS.h>
#include <task.h>

#include <hardware/adc.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/timer.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include <lwip/apps/httpd.h>
#include <lwip/apps/mdns.h>
#include <lwip/init.h>
#include <lwip/ip4_addr.h>

#include "rtoshooks.h"
#include "states.h"
#include "tSound.h"

#include "Context.h"
#include "pindefs.h"

Context ctx;
QueueHandle_t soundQueue;

uint32_t micros32(void) {
	return time_us_32(); // Calls the SDK's static inline function
}

void tFoto(void *pvParameters) {
	while (1) {
		ctx.fotoValue = adc_read();
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void tLoop(void *pvParameters) {
	while (1) {
		printf("[Supervisor] trying to get mutex\n");
		if (xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) == pdTRUE) {
			printf("[Supervisor] took mutex\n");
			if (ctx.currentTask) {
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
					configMINIMAL_STACK_SIZE * 2,
					NULL,
					tskIDLE_PRIORITY + 8,
					&ctx.currentTask
				);
				break;
			}
			xSemaphoreGive(ctx.taskMutex);
		}
	}
}


void setupPins(void *pvParameters) {
	ctx.rgb.init(LED_R, LED_G, LED_B, true);
	ctx.lcd.init(i2c0, 0x27, LCD_SDA, LCD_SCL);
	ctx.redButton.init(RED_BTN, false, 50);
	ctx.yellowButton.init(YELLOW_BTN, false, 50);
	ctx.resetButton.init(GAME_RST_BTN, false, 50);
	initSound();
	adc_init();
	adc_gpio_init(FOTORESISTOR);
	adc_select_input(0);
	ctx.fotoValue = adc_read();
	ctx.rfid.init(
		spi0,
		RFID_CS,
		RFID_RST,
		RFID_IRQ,
		RFID_MISO,
		RFID_SCK,
		RFID_MOSI
	);

	ctx.gameState = GameState::MAIN;
	ctx.taskMutex = xSemaphoreCreateBinary();
	xSemaphoreGive(ctx.taskMutex);
	soundQueue = xQueueCreate(8, sizeof(SoundEffect));
	register_cli_commands();

	xTaskCreate(
		vCommandConsoleTask,
		"CLI",
		1024,
		NULL,
		tskIDLE_PRIORITY + 1,
		NULL
	);

	xTaskCreate(
		tFoto,
		"tFoto",
		configMINIMAL_STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY,
		NULL
	);

	xTaskCreate(tLoop, "tLoop", 8192, NULL, tskIDLE_PRIORITY + 1, NULL);

	while(1){
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

int main() {
	stdio_init_all();
	while (!stdio_usb_connected()) {
		sleep_ms(100);
	}
	
	xTaskCreate(
		setupPins,
		"setupPins",
		configMINIMAL_STACK_SIZE * 2,
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
