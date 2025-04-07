#include <format>

#include "cJSON.h"

#include "PlayerDao.hpp"
#include "Game.hpp"

char *Game::toJSON() {
	cJSON *game = cJSON_CreateObject();
	cJSON_AddNumberToObject(game, "player1", player1.uid);
	cJSON_AddNumberToObject(game, "player2", player2.uid);
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

void Game::save() {
}

void Game::addRound(bool player, uint32_t p1_us, uint32_t p2_us) {
	rounds->push_back({player, p1_us, p2_us});
}

Player Game::getPlayer(bool i) const { return i==0 ? player1 : player2; }

void Game::setPlayer(bool i, uint32_t uid) {
    Player p = PlayerDao::getPlayer(uid);
	if (i==0) {
		player1 = p;
	} else {
		player2 = p;
	}
}

Game::Game(uint32_t p1uid, uint32_t p2uid) {
    this->player1 = PlayerDao::getPlayer(p1uid);
    this->player2 = PlayerDao::getPlayer(p2uid);
	rounds = std::make_unique<std::vector<Round>>();
}