#include "Game.h"
extern "C" {
  #include <hardware/sync.h>
  #include <hardware/flash.h>
};
#include "pico/stdlib.h"
#include "Context.h"
#include <cstring>
#include "pico/multicore.h"
#include "pico/flash.h"
#include <stdio.h>
#include "pindefs.h"


#define RESERVED_FLASH_SIZE  (8 * 1024)
#define RESERVED_FLASH_START ((2*1024*1024)-RESERVED_FLASH_SIZE) // 2MB flash, reserved 32kB for NVMem
const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + RESERVED_FLASH_START);

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
	const void *src = (const void *)(flash_target_contents);
    memcpy(&(ctx.nvmem), src, sizeof(NVMem));
	if (ctx.nvmem.magic != NVMEM_MAGIC) {
		printf("[DAO] NVMem magic mismatch, initializing new NVMem\n");
		ctx.nvmem.magic = NVMEM_MAGIC;
		ctx.nvmem.numPlayers = 0;
		ctx.nvmem.numGames = 0;
		memset(ctx.nvmem.players, 0, sizeof(ctx.nvmem.players));
		memset(ctx.nvmem.games, 0, sizeof(ctx.nvmem.games));
	} else {
		printf("[DAO] Loaded NVMem with %d players and %d games\n", ctx.nvmem.numPlayers, ctx.nvmem.numGames);
	}
}

static void call_flash_range_erase(void *param) {
    uint32_t offset = (uint32_t)param;
    flash_range_erase(offset, RESERVED_FLASH_SIZE);
}
static void call_flash_range_program(void *param) {
    uint32_t offset = ((uintptr_t*)param)[0];
    const uint8_t *data = (const uint8_t *)((uintptr_t*)param)[1];
    flash_range_program(offset, data, RESERVED_FLASH_SIZE);
}
void print_buf(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%02x", buf[i]);
        if (i % 64 == 63)
            printf("\n");
        else
            printf(" ");
    }
}
void saveGames() {
	printf("[DAO] Saving games to flash...\n");
	printf("[DAO] NVMem size: %zu bytes\n", sizeof(NVMem));
	int rc = flash_safe_execute(call_flash_range_erase, (void*)RESERVED_FLASH_START, UINT32_MAX);
    hard_assert(rc == PICO_OK);
	printf("[DAO] Flash erased successfully\n");

	printf("[DAO] Writing to flash...\n");
	uintptr_t params[] = { RESERVED_FLASH_START, (uintptr_t)&(ctx.nvmem) };
    rc = flash_safe_execute(call_flash_range_program, params, UINT32_MAX);
    hard_assert(rc == PICO_OK);
	printf("[DAO] Flash written successfully\n");
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