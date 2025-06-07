#include "rtoshooks.h"
#include "Context.h"
#include <cstdlib>

BaseType_t rename_command(char *write_buffer, size_t write_buffer_len, const char *command) {
    const char *uid_param = FreeRTOS_CLIGetParameter(command, 1, NULL);
    const char *name_param = FreeRTOS_CLIGetParameter(command, 2, NULL);

    write_buffer[0] = '\0';

    if (!uid_param || !name_param) {
        snprintf(write_buffer, write_buffer_len, "Usage: rename 0x<uid> <name>\n");
        return pdFALSE;
    }

    if(ctx.nvmem.magic != NVMEM_MAGIC) {
        snprintf(write_buffer, write_buffer_len, "NVMem is corrupted\n");
        return pdFALSE;
    }

    // Example: parse uid as integer and simulate renaming logic
    // Accept UID in hex (with or without 0x prefix)
    uint32_t uid = 0;
    if (strncasecmp(uid_param, "0x", 2) == 0) {
        uid = strtoul(uid_param, NULL, 16);
    } else {
        uid = strtoul(uid_param, NULL, 0);
    }
    if (uid == 0) {
        snprintf(write_buffer, write_buffer_len, "Invalid UID: %s\n", uid_param);
        return pdFALSE;
    }
    const char *new_name = name_param;

    for (uint32_t i = 0; i < ctx.nvmem.numPlayers; i++) {
        if (ctx.nvmem.players[i].uid == uid) {
            // Update existing player's name
            strncpy(ctx.nvmem.players[i].name, new_name, (PLAYER_NAME_SIZE*4) - 1);
            ctx.nvmem.players[i].name[(PLAYER_NAME_SIZE*4) - 1] = '\0'; // Ensure null termination
            snprintf(write_buffer, write_buffer_len, "Renamed player (UID:0x%08X) to \"%s\"\n", uid, new_name);
            return pdFALSE;
        }
    }

    ctx.nvmem.players[ctx.nvmem.numPlayers].uid = uid;
    strncpy(ctx.nvmem.players[ctx.nvmem.numPlayers].name, new_name, (PLAYER_NAME_SIZE*4) - 1);
    ctx.nvmem.players[ctx.nvmem.numPlayers].name[(PLAYER_NAME_SIZE*4) - 1] = '\0'; // Ensure null termination
    ctx.nvmem.numPlayers++;
    snprintf(write_buffer, write_buffer_len, "Created player (UID:0x%08X) with name \"%s\"\n", uid, new_name);

    return pdFALSE;
}
