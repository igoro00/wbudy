#pragma once

#include <cstdint>
#include "Player.hpp"

class PlayerDao {
    private:
        static void addPlayer(uint32_t uid);
    public:
        static const Player getPlayer(uint32_t uid);
};