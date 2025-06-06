#pragma once

#include <memory>
#include <vector>

#include "Player.hpp"

struct Round {
	bool player;
	uint32_t p1_us;
	uint32_t p2_us;
};

// const Player players[4] = {
// 	{0, L"Unknown"},
// 	{0xEAAA764F, L"igoro00"},
// 	{0x3A4AE9CE, L"Adzik"},
// 	{0x5A7EFF4B, L"Detweiler"}
// };

class Game {
private:
	Player player1;
	Player player2;
	std::unique_ptr<std::vector<Round>> rounds;
public:
	Game(uint32_t p1uid, uint32_t p2uid);
	Player getPlayer(bool i) const;
	void setPlayer(bool i, uint32_t uid);
	void addRound(bool player, uint32_t p1_us, uint32_t p2_us);
	const char* toJSON();
	void save();
};