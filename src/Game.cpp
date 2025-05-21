#include <format>

// #include "FatFS.h"

#include "Game.hpp"
#include "PlayerDao.hpp"

// const char *Game::toJSON() {
// 	cJSON *game = cJSON_CreateObject();
// 	cJSON_AddNumberToObject(game, "player1", player1.uid);
// 	cJSON_AddNumberToObject(game, "player2", player2.uid);
// 	cJSON *roundsArray = cJSON_CreateArray();
// 	for (const auto &round : *rounds) {
// 		cJSON *roundObj = cJSON_CreateArray();
        
//         cJSON_AddItemToArray(roundObj, cJSON_CreateNumber(round.player));

//         if(round.p1_us > 0) {
//             cJSON_AddItemToArray(roundObj, cJSON_CreateNumber(round.p1_us-1));
//         } else {
//             cJSON_AddItemToArray(roundObj, cJSON_CreateNull());
//         }
//         if(round.p2_us > 0) {
//             cJSON_AddItemToArray(roundObj, cJSON_CreateNumber(round.p2_us-1));
//         } else {
//             cJSON_AddItemToArray(roundObj, cJSON_CreateNull());
//         }
// 		cJSON_AddItemToArray(roundsArray, roundObj);
// 	}
// 	cJSON_AddItemToObject(game, "rounds", roundsArray);
// 	const char *out = cJSON_PrintUnformatted(game);
// 	cJSON_Delete(game);
// 	return out;
// }

// saves file in /games/<gameIndex>.json and adds entry to /gamesIndex
void Game::save() {
	uint16_t gameIndex = 0;

	// fs::Dir dir = FatFS.openDir("/games");
	// while (dir.next()) {
	// 	// probably never gonna happen
	// 	if (gameIndex >= UINT16_MAX) {
	// 		return;
	// 	}
	// 	gameIndex++;
	// }

	// fs::File f = FatFS.open(
	// 	std::format(
	// 		"/games/{:05}.json",
	// 		gameIndex
	// 	)
	// 		.c_str(),
	// 	"w+"
	// );
	// f.write(toJSON());
    // f.close();
	// f = FatFS.open(
	// 	std::format(
	// 		"/gamesIndex",
	// 		gameIndex
	// 	)
	// 		.c_str(),
	// 	"a"
	// );
	// f.write(
	// 	std::format(
	// 		"{:08X}_{:08X}_{:05}\n",
	// 		player1.uid,
	// 		player2.uid,
	// 		gameIndex
	// 	).c_str()
	// );
	// f.close();
}

void Game::addRound(bool player, uint32_t p1_us, uint32_t p2_us) {
	rounds->push_back({player, p1_us, p2_us});
}

Player Game::getPlayer(bool i) const { return i == 0 ? player1 : player2; }

void Game::setPlayer(bool i, uint32_t uid) {
	Player p = PlayerDao::getPlayer(uid);
	if (i == 0) {
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