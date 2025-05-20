#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/apps/httpd.h"
#include "lwip/apps/mdns.h"
#include "lwip/init.h"
#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"

void main_task(__unused void *params) {
	if (cyw43_arch_init()) {
		printf("failed to initialise\n");
		return;
	}

	while (true) {
		vTaskDelay(100);
	}

	cyw43_arch_deinit();
}

int main() {
	stdio_init_all();

	TaskHandle_t task;
	xTaskCreate(
		main_task,
		"TestMainThread",
		configMINIMAL_STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY + 1,
		&task
	);

	// we must bind the main task to one core (well at least while the init is
	// called) (note we only do this in NO_SYS mode, because cyw43_arch_freertos
	// takes care of it otherwise)
	vTaskCoreAffinitySet(task, 1);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
