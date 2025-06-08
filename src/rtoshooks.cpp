#include <cstring>
#include <stdio.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "Context.h"
#include "Game.h"
#include "pindefs.h"
#include "rtoshooks.h"

extern "C" void
vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
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

extern "C" void vApplicationPassiveIdleHook() {
	// Optional: Add low-power code here
}

extern "C" void HardFault_Handler() {
	// Inspect SCB->HFSR, stacked registers, or custom diagnostics
	vTaskSuspendAll();
	multicore_lockout_start_blocking();
	while (1) {
		gpio_put(LED_R, 1); // Turn on red LED
		sleep_ms(1000);
		gpio_put(LED_R, 0); // Turn off red LED
		sleep_ms(1000);
		printf("Hard Fault occurred! Check the device.\n");
		// Optionally, you can reset the device here
		// NVIC_SystemReset();
	}
}

#define MAX_INPUT_LENGTH 100
#define MAX_OUTPUT_LENGTH 2048

void vCommandConsoleTask(void *params) {
	char input[MAX_INPUT_LENGTH];
	char output[MAX_OUTPUT_LENGTH];
	int input_index = 0;

	printf("> "); // prompt

	while (1) {
		int c = getchar_timeout_us(0);
		if (c == PICO_ERROR_TIMEOUT) {
			vTaskDelay(pdMS_TO_TICKS(10));
			continue;
		}

		if (c == '\r' || c == '\n') {
			putchar('\r');
			putchar('\n');
			input[input_index] = '\0';

			BaseType_t more_output;
			do {
				more_output = FreeRTOS_CLIProcessCommand(
					input,
					output,
					MAX_OUTPUT_LENGTH
				);
				printf("%s", output);
			} while (more_output != pdFALSE);

			input_index = 0;
			printf("> ");
		} else if (c == '\b' && input_index > 0) {
			input_index--;
			printf("\b \b");
		} else if (input_index < MAX_INPUT_LENGTH - 1) {
			input[input_index++] = c;
			putchar(c);
		}
	}
}



void register_cli_commands() {
	FreeRTOS_CLIRegisterCommand(&hello_definition);
	FreeRTOS_CLIRegisterCommand(&top_definition);
	FreeRTOS_CLIRegisterCommand(&cat_definition);
	FreeRTOS_CLIRegisterCommand(&rm_definition);
    FreeRTOS_CLIRegisterCommand(&clear_definition);
    FreeRTOS_CLIRegisterCommand(&cls_definition);
    FreeRTOS_CLIRegisterCommand(&load_definition);
    FreeRTOS_CLIRegisterCommand(&save_definition);
    FreeRTOS_CLIRegisterCommand(&rename_definition);
}