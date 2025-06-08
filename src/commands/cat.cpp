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
		size_t p1len = strlen(getPlayerName(game->players[0]));
		size_t p2len = strlen(getPlayerName(game->players[1]));
		int longer_len = p1len > p2len ? p1len : p2len;
		int32_t largestP1 = INT32_MIN;
		int32_t largestP2 = INT32_MIN;
		for (int i = 0; i < sizeof(game->rounds) / sizeof(Round); i++) {
			if (game->rounds[i].p1_us > largestP1) {
				largestP1 = game->rounds[i].p1_us;
			}
			if (game->rounds[i].p2_us > largestP2) {
				largestP2 = game->rounds[i].p2_us;
			}
		}
		int longestP1 = snprintf(NULL, 0, "%d", largestP1);
		int longestP2 = snprintf(NULL, 0, "%d", largestP2);
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
				"\t\tRound %2d: Player: %*s (UID: 0x%08X) p1_us: %*d p2_us: %*d",
				i + 1,
				longer_len,
				getPlayerName(game->players[game->rounds[i].player-1]),
				game->players[game->rounds[i].player-1],
				longestP1,
				game->rounds[i].p1_us,
				longestP2,
				game->rounds[i].p2_us
			);
			if(game->rounds[i].p1_us == UINT32_MAX ||
			   game->rounds[i].p2_us == UINT32_MAX) {
				n += snprintf(
					write_buffer + n,
					write_buffer_len - n,
					" (False start)\n"
				);
			} else if (game->rounds[i].p1_us == 0 &&
				game->rounds[i].p2_us == 0) {
				n += snprintf(
					write_buffer + n,
					write_buffer_len - n,
					" (Timeout)\n"
				);
			} else if (game->rounds[i].p1_us == game->rounds[i].p2_us) {
				n += snprintf(
					write_buffer + n,
					write_buffer_len - n,
					" (Draw)\n"
				);
			} else {
				uint32_t p1 = game->rounds[i].p1_us==0 ? UINT32_MAX : game->rounds[i].p1_us;
				uint32_t p2 = game->rounds[i].p2_us==0 ? UINT32_MAX : game->rounds[i].p2_us;
				if (p1 < p2) { // Player 1 was faster
					if (game->rounds[i].player == 1) { // Player 1 should've been faster
						n += snprintf(
							write_buffer + n,
							write_buffer_len - n,
							" (Player 1 won)\n"
						);
					} else if (game->rounds[i].player == 2) {
						n += snprintf(
							write_buffer + n,
							write_buffer_len - n,
							" (Player 1 lost)\n"
						);
					}
				} else if (p2 < p1) { // Player 2 was faster
					if (game->rounds[i].player == 1) { // Player 1 should've been faster
						n += snprintf(
							write_buffer + n,
							write_buffer_len - n,
							" (Player 2 lost)\n"
						);
					} else if (game->rounds[i].player == 2) {
						n += snprintf(
							write_buffer + n,
							write_buffer_len - n,
							" (Player 2 won)\n"
						);
					}
				}
			}
		}
		step++;
		return pdTRUE;
	} else {
		snprintf(write_buffer, write_buffer_len, "Unknown argument\n");
	}
	return pdFALSE;
}
