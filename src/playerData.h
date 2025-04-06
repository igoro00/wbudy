#pragma once
#include "cJSON.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct Player {
	uint32_t uid;
	wchar_t name[12];
};

struct Round {
	uint32_t p1_us;
	uint32_t p2_us;
};

const Player players[4] = {
	{0, L"Unknown"},
	{0xEAAA764F, L"igoro00"},
	{0x3A4AE9CE, L"Adzik"},
	{0x5A7EFF4B, L"Detweiler"}
};

class Game {
	uint32_t player1;
	uint32_t player2;
	std::unique_ptr<std::vector<Round>> rounds;
public:
	Game(uint32_t player1, uint32_t player2) : player1(player1), player2(player2) {
		rounds = std::make_unique<std::vector<Round>>();
	}

	uint32_t getPlayer(bool i) const {
		return i ? player1 : player2;
	}

	void setPlayer(bool i, uint32_t uid) {
		if (i) {
			player1 = uid;
		} else {
			player2 = uid;
		}
	}

	const wchar_t* getPlayerName(bool i) const {
		uint32_t uid = i ? player1 : player2;

		for (const auto& player : players) {
			if (player.uid == uid) {
				return player.name;
			}
		}

		// player uid as hex
		return std::format(L"{:x}", uid).c_str();
	}

	void addRound(uint32_t p1_us, uint32_t p2_us) {
		rounds->push_back({p1_us, p2_us});
	}

	char* toJSON() {
		cJSON* game = cJSON_CreateObject();
		cJSON_AddNumberToObject(game, "player1", player1);
		cJSON_AddNumberToObject(game, "player2", player2);
		cJSON* roundsArray = cJSON_CreateArray();
		for (const auto& round : *rounds) {
			cJSON* roundObj = cJSON_CreateArray();
			cJSON* p1_us = cJSON_CreateNumber(round.p1_us);
			cJSON* p2_us = cJSON_CreateNumber(round.p2_us);
			cJSON_AddItemToArray(roundObj, p1_us);
			cJSON_AddItemToArray(roundObj, p2_us);
		}
		cJSON_AddItemToObject(game, "rounds", roundsArray);
		char *out = cJSON_Print(game);
		cJSON_Delete(game);
		return out;
	}
};