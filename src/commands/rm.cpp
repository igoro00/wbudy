#include "Context.h"
#include "rtoshooks.h"

BaseType_t rm_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command
) {
	const char *arg;
	write_buffer[0] = '\0';

	arg = FreeRTOS_CLIGetParameter(command, 1, NULL);
	if (!arg) {
		snprintf(write_buffer, write_buffer_len, "Missing argument\n");
		return pdFALSE;
	}
	if (ctx.nvmem.magic != NVMEM_MAGIC) {
		snprintf(write_buffer, write_buffer_len, "NVMem is corrupted\n");
		return pdFALSE;
	}
	if (strncmp(arg, "players", 7) == 0) {
		ctx.nvmem.currentPlayer = 0;
		memset(ctx.nvmem.players, 0, sizeof(ctx.nvmem.players));
	} else if (strncmp(arg, "games", 5) == 0) {
		ctx.nvmem.currentGame = 0;
		memset(ctx.nvmem.games, 0, sizeof(ctx.nvmem.games));
	} else if (strncmp(arg, "*", 1) == 0) {
		ctx.nvmem.currentPlayer = 0;
		ctx.nvmem.currentGame = 0;
		memset(ctx.nvmem.players, 0, sizeof(ctx.nvmem.players));
		memset(ctx.nvmem.games, 0, sizeof(ctx.nvmem.games));
	} else {
		snprintf(write_buffer, write_buffer_len, "Unknown argument\n");
	}
	return pdFALSE;
}
