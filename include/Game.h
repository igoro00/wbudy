#pragma once

#include <memory>
#include <vector>

static const uint32_t NVMEM_MAGIC = 0x52464C58; // 'RFLX'

struct Round {
	// najmniejszy rozmiar to 4B wiec nie ważne
	// czy bool czy uint8_t czy uint32_t
	// 0x00 - player1
	// 0x01 - player2
	// 0xFF - nie rozegrano rundy
	uint8_t player; 
	uint32_t p1_us;
	uint32_t p2_us;
};

#define PLAYER_NAME_SIZE 12
struct Player {
	uint32_t uid;
	char name[PLAYER_NAME_SIZE*4];
};

struct Game {
	uint32_t players[2];
	Round rounds[10];
};

struct NVMem { // 32k
	uint32_t magic;
	uint32_t numPlayers; // last player index
	uint32_t numGames; // last game index
	Player players[49];
	Game games[236];
};


const Player players[4] = {
	{0xEAAA764F, "igoro00"},
	{0x3A4AE9CE, "Adzik"},
	{0x5A7EFF4B, "Detweiler"}
};

void loadGames();
void saveGames();
Game* getNewGame();
const char* getPlayerName(uint32_t uid);