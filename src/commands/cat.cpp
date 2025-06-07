#include "rtoshooks.h"
#include "Context.h"

// Jeśli zwrócisz pdTRUE to funkcja zostanie wywołana ponownie
// ale step bedzie zachowany.
// Wysyłamy w jednym wywołaniu tylko jedną grę/gracza żeby oszczędzać na buforze
// (tylko 1024 bajty). Jeśli zwrócisz pdFALSE to funkcja nie będzie wywoływana
// ponownie.
BaseType_t
cat_command(char *write_buffer, size_t write_buffer_len, const char *command) {
	static int step = 0;
	const char *arg;
	write_buffer[0] = '\0';

	arg = FreeRTOS_CLIGetParameter(command, 1, NULL);
	if (!arg) {
		snprintf(write_buffer, write_buffer_len, "Missing argument\n");
		return pdFALSE;
	}
	if (ctx.nvmem.magic != NVMEM_MAGIC) {
		snprintf(
			write_buffer,
			write_buffer_len,
			"NVMem is corrupted\n"
		);
		return pdFALSE;
	}
	if (strncmp(arg, "players", 7) == 0) {
		if (step >= ctx.nvmem.currentPlayer) {
			snprintf(
				write_buffer,
				write_buffer_len,
				"Total players: %d\n",
				ctx.nvmem.currentPlayer
			);
			step = 0; // reset step for next command
			return pdFALSE;
		}
		snprintf(
			write_buffer,
			write_buffer_len,
			"Player %d: UID: 0x%08X, Name: %s\n",
			step + 1,
			ctx.nvmem.players[step].uid,
			ctx.nvmem.players[step].name
		);
		step++;
		return pdTRUE;
	} else if (strncmp(arg, "games", 5) == 0) {
		if (step >= ctx.nvmem.currentGame) {
			snprintf(
				write_buffer,
				write_buffer_len,
				"Total games: %d\n",
				ctx.nvmem.currentGame
			);
			step = 0; // reset step for next command
			return pdFALSE;
		}
		Game *game = &ctx.nvmem.games[step];
		size_t n = 0;
		n += snprintf(
			write_buffer + n,
			write_buffer_len - n,
			"Game %d:\n",
			step + 1
		);
		n += snprintf(
			write_buffer + n,
			write_buffer_len - n,
			"\tPlayer1: %s (UID: 0x%08X)\n",
			getPlayerName(game->players[0]),
			game->players[0]
		);
		n += snprintf(
			write_buffer + n,
			write_buffer_len - n,
			"\tPlayer2: %s (UID: 0x%08X)\n",
			getPlayerName(game->players[1]),
			game->players[1]
		);
        n += snprintf(
            write_buffer + n,
            write_buffer_len - n,
            "\tRounds:\n"
        );
		for (int i = 0; i < sizeof(game->rounds) / sizeof(Round); i++) {
			n += snprintf(
				write_buffer + n,
				write_buffer_len - 1,
				"\t\tRound %2d: Player: %s (UID: 0x%08X), p1_us: %d, p2_us: %d\n",
				i + 1,
				getPlayerName(game->rounds[i].player),
				game->rounds[i].p1_us,
				game->rounds[i].p2_us
			);
		}
		step++;
		return pdTRUE;
	} else {
		snprintf(write_buffer, write_buffer_len, "Unknown argument\n");
	}
	return pdFALSE;
}
