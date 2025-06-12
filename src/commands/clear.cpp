#include "rtoshooks.h"

BaseType_t clear_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command
) {
	// ANSI escape sequence to clear screen and move cursor to top-left
	snprintf(write_buffer, write_buffer_len, "\x1b[2J\x1b[H");
	return pdFALSE;
}
