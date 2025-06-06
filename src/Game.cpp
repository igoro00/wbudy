#include "Game.h"
#include "hardware/flash.h"
#include "pico/stdlib.h"
#include "Context.h"
#include <cstring>
#include "pico/multicore.h"
#include <stdio.h>

#define RESERVED_FLASH_START 0x101F8000
#define RESERVED_FLASH_SIZE  (32 * 1024)

// void Game::addRound(bool player, uint32_t p1_us, uint32_t p2_us) {
// 	rounds->push_back({player, p1_us, p2_us});
// }

// Player Game::getPlayer(bool i) const { return i == 0 ? player1 : player2; }

// void Game::setPlayer(bool i, uint32_t uid) {
// 	Player p = PlayerDao::getPlayer(uid);
// 	if (i == 0) {
// 		player1 = p;
// 	} else {
// 		player2 = p;
// 	}
// }

// Game::Game(uint32_t p1uid, uint32_t p2uid) {
// 	this->player1 = PlayerDao::getPlayer(p1uid);
// 	this->player2 = PlayerDao::getPlayer(p2uid);
// 	rounds = std::make_unique<std::vector<Round>>();
// }

void loadGames() {
	const void *src = (const void *)RESERVED_FLASH_START;
    memcpy(&(ctx.nvmem), src, sizeof(NVMem));
	if (ctx.nvmem.magic != NVMEM_MAGIC) {
		printf("[Game] NVMem magic mismatch, initializing new NVMem\n");
		ctx.nvmem.magic = NVMEM_MAGIC;
		ctx.nvmem.numPlayers = 0;
		ctx.nvmem.numGames = 0;
		memset(ctx.nvmem.players, 0, sizeof(ctx.nvmem.players));
		memset(ctx.nvmem.games, 0, sizeof(ctx.nvmem.games));
	}
}

void saveGames() {
	multicore_lockout_start_blocking();
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(RESERVED_FLASH_START - XIP_BASE, FLASH_SECTOR_SIZE); // sector-aligned
    flash_range_program(RESERVED_FLASH_START - XIP_BASE, (const uint8_t *)&(ctx.nvmem), sizeof(NVMem));
    restore_interrupts(ints);
	multicore_lockout_end_blocking();
}

const char *getPlayerName(uint32_t uid) { 
	for (uint32_t i = 0; i < ctx.nvmem.numPlayers; i++) {
		if (ctx.nvmem.players[i].uid == uid) {
			return ctx.nvmem.players[i].name;
		}
	}
	return "Unknown";
}

Game* getNewGame() {
	return &(ctx.nvmem.games[ctx.nvmem.numGames++]);
}