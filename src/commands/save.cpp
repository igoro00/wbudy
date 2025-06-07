#include "rtoshooks.h"
#include "Game.h"

BaseType_t save_command(char *write_buffer, size_t write_buffer_len, const char *command) {
    saveGames();
    return pdFALSE;
}