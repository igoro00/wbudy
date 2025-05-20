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

#include "task.h"

#include "Context.hpp"

Context ctx;

void setupPins(){

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
		printf("Exited Scheduler\n");
		sleep_ms(1000);
	}
}
