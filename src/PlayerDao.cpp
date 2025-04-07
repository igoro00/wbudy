#include <format>

#include "FatFS.h"

#include "PlayerDao.hpp"
#include "sysutils.hpp"

void PlayerDao::addPlayer(uint32_t uid) {
    fs::File f = FatFS.open(std::format("/players/{:x}", uid).c_str(), "w+");
    if (!f) {
        Serial.println("Failed to open file for writing");
        myPanic();
    }
    f.write((const char*)std::format(L"{:x}", uid).c_str());
    f.close();
}

const Player PlayerDao::getPlayer(uint32_t uid) {
    Player player;
    player.uid = uid;
    fs::File f = FatFS.open(std::format("/players/{:x}", uid).c_str(), "r");
    if (!f) {
        PlayerDao::addPlayer(uid);
        wcscpy(player.name, std::format(L"{:x}", uid).c_str());
        return player;
    }
    f.read((uint8_t*)player.name, sizeof(player.name));
    f.close();
    return player;
}