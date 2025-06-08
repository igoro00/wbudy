#include "rtoshooks.h"

BaseType_t hello_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command_string
) {
	snprintf(write_buffer, write_buffer_len, "Hello from CLI!\r\n");
	return pdFALSE;
}