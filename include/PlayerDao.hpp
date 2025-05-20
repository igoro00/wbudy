#include "Player.hpp"
#include "cstdint"
class PlayerDao {
    private:
        static void addPlayer(uint32_t uid);
    public:
        static const Player getPlayer(uint32_t uid);
};