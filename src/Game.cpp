#include <format>

#include "cJSON.h"

#include "Game.hpp"

char *Game::toJSON() {
	cJSON *game = cJSON_CreateObject();
	cJSON_AddNumberToObject(game, "player1", player1);
	cJSON_AddNumberToObject(game, "player2", player2);
	cJSON *roundsArray = cJSON_CreateArray();
	for (const auto &round : *rounds) {
		cJSON *roundObj = cJSON_CreateArray();
		cJSON *p1_us = cJSON_CreateNumber(round.p1_us);
		cJSON *p2_us = cJSON_CreateNumber(round.p2_us);
		cJSON_AddItemToArray(roundObj, p1_us);
		cJSON_AddItemToArray(roundObj, p2_us);
	}
	cJSON_AddItemToObject(game, "rounds", roundsArray);
	char *out = cJSON_Print(game);
	cJSON_Delete(game);
	return out;
}

const wchar_t *Game::getPlayerName(bool i) const {
	uint32_t uid = i ? player1 : player2;

	for (const auto &player : players) {
		if (player.uid == uid) {
			return player.name;
		}
	}

	// player uid as hex
	return std::format(L"{:x}", uid).c_str();
}

void Game::addRound(uint32_t p1_us, uint32_t p2_us) {
	rounds->push_back({p1_us, p2_us});
}

uint32_t Game::getPlayer(bool i) const { return i ? player1 : player2; }

void Game::setPlayer(bool i, uint32_t uid) {
	if (i) {
		player1 = uid;
	} else {
		player2 = uid;
	}
}

Game::Game(uint32_t player1, uint32_t player2)
	: player1(player1), player2(player2) {
	rounds = std::make_unique<std::vector<Round>>();
}