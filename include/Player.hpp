#pragma once

#include <cstdint>

#define PLAYER_NAME_SIZE 12

struct Player {
	uint32_t uid;
	wchar_t name[PLAYER_NAME_SIZE];
};
