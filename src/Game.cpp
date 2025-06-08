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
		ctx.nvmem.currentPlayer = 0;
		ctx.nvmem.currentGame = 0;
		memset(ctx.nvmem.players, 0, sizeof(ctx.nvmem.players));
		memset(ctx.nvmem.games, 0, sizeof(ctx.nvmem.games));
		ctx.nvmem.players[0].uid = 0xEAAA764F;
		strcpy(ctx.nvmem.players[0].name, "igoro00");
		ctx.nvmem.players[1].uid = 0x3A4AE9CE;
		strcpy(ctx.nvmem.players[1].name, "Adzik");
		ctx.nvmem.players[2].uid = 0x5A7EFF4B;
		strcpy(ctx.nvmem.players[2].name, "Detweiler");
	} else {
		printf("[DAO] Loaded NVMem with %d players and %d games\n", ctx.nvmem.currentPlayer, ctx.nvmem.currentGame);
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
	for (uint32_t i = 0; i < ctx.nvmem.currentPlayer; i++) {
		if (ctx.nvmem.players[i].uid == uid) {
			return ctx.nvmem.players[i].name;
		}
	}
	return "Unknown";
}


// templated map
template<typename T>
inline T map(T value, T in_min, T in_max, T out_min, T out_max) {
	if (in_min == in_max) return out_min; // avoid division by zero
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<typename T>
inline T clamp(T value, T min, T max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}


// i didnt know where to put it
// we dont have utils.cpp
// and i dont want to create a full lib for this
uint8_t FotoToL(uint16_t fotoValue){
	return clamp<int32_t>(map<int32_t>((int32_t)fotoValue, FOTO_MIN, FOTO_MAX, 8, 128), 8, 128);
}