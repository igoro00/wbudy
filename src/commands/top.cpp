#include "rtoshooks.h"

BaseType_t top_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command_string
) {
	char *p = write_buffer;
	int n = 0;

	n += snprintf(
		p + n,
		write_buffer_len - n,
		"\nTask          State  Prio Stack Num\n"
		"***********************************\n"
	);
	vTaskList(p + n);
	n = strlen(write_buffer); // update length after vTaskList writes

	n += snprintf(
		p + n,
		write_buffer_len - n,
		"\nTask            Abs Time    CPU%%\n"
		"***********************************\n"
	);
	vTaskGetRunTimeStats(p + n);

	return pdFALSE;
}