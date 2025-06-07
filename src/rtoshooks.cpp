#include <stdio.h>
#include <stdarg.h>
#include <cstring>
#include <pico/stdlib.h>
#include <pico/multicore.h>


#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <FreeRTOS_CLI.h>

#include "rtoshooks.h"
#include "pindefs.h"

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
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
#define MAX_OUTPUT_LENGTH 1024

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
                more_output = FreeRTOS_CLIProcessCommand(input, output, MAX_OUTPUT_LENGTH);
                printf("%s", output);
            } while (more_output != pdFALSE);

            input_index = 0;
            printf("> ");
        } else if (c == '\b' && input_index > 0) {
            input_index--;
            printf("\b \b");
        } else if (input_index < MAX_INPUT_LENGTH - 1 && c >= 32 && c <= 126) {
            input[input_index++] = c;
            putchar(c);
        }
    }
}

static BaseType_t hello_cmd(char *write_buffer, size_t write_buffer_len, const char *command_string) {
    snprintf(write_buffer, write_buffer_len, "Hello from CLI!\r\n");
    return pdFALSE;
}

static const CLI_Command_Definition_t hello_command = {
    "hello",
    "hello: Prints hello message\r\n",
    hello_cmd,
    0
};

static BaseType_t top_cmd(char *write_buffer, size_t write_buffer_len, const char *command_string) {
    char *p = write_buffer;
    int n = 0;

    n += snprintf(p + n, write_buffer_len - n,
        "\nTask          State  Prio Stack Num\n"
        "***********************************\n");
    vTaskList(p + n);
    n = strlen(write_buffer); // update length after vTaskList writes

    n += snprintf(p + n, write_buffer_len - n,
        "\nTask            Abs Time    CPU%%\n"
        "***********************************\n");
    vTaskGetRunTimeStats(p + n);

    return pdFALSE;
}

static const CLI_Command_Definition_t top_command = {
    "top",
    "top: returns list of running tasks and their \%cpu\r\n",
    top_cmd,
    0
};


void register_cli_commands() {
    FreeRTOS_CLIRegisterCommand(&hello_command);
    FreeRTOS_CLIRegisterCommand(&top_command);
}