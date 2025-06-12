#pragma once

#include <memory>
#include <vector>

static const uint32_t NVMEM_MAGIC = 0x52464C58; // 'RFLX'

struct Round {
	// najmniejszy rozmiar to 4B wiec nie ważne
	// czy bool czy uint8_t czy uint32_t
	//
	// 0x00 - nie rozegrano rundy
	// 0x01 - player1
	// 0x02 - player2
	uint8_t player;
	int32_t p1_us;
	int32_t p2_us;
};

#define PLAYER_NAME_SIZE 12
struct Player {
	uint32_t uid;
	char name[PLAYER_NAME_SIZE * 4];
};

struct Game {
	uint32_t players[2];
	Round rounds[10];
};

struct NVMem { // 32k
	uint32_t magic;
	uint32_t currentPlayer;
	uint32_t currentGame;
	Player players[49];
	Game games[236];
};

void loadGames();
void saveGames();
const char *getPlayerName(uint32_t uid);