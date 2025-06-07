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

    if (strlen(name_param) >= (PLAYER_NAME_SIZE * 4)) {
        snprintf(write_buffer, write_buffer_len, "Name too long, max length is %dB\n", (PLAYER_NAME_SIZE * 4) - 1);
        return pdFALSE;
    }

    for (uint32_t i = 0; i < ctx.nvmem.currentPlayer; i++) {
        if (ctx.nvmem.players[i].uid == uid) {
            // Update existing player's name
            strcpy(ctx.nvmem.players[i].name, name_param);
            ctx.nvmem.players[i].name[(PLAYER_NAME_SIZE*4) - 1] = '\0'; // Ensure null termination
            snprintf(write_buffer, write_buffer_len, "Renamed player (UID:0x%08X) to \"%s\"\n", uid, name_param);
            return pdFALSE;
        }
    }

    ctx.nvmem.players[ctx.nvmem.currentPlayer].uid = uid;
    strcpy(ctx.nvmem.players[ctx.nvmem.currentPlayer].name, name_param);
    ctx.nvmem.players[ctx.nvmem.currentPlayer].name[(PLAYER_NAME_SIZE*4) - 1] = '\0'; // Ensure null termination
    ctx.nvmem.currentPlayer++;
    snprintf(write_buffer, write_buffer_len, "Created player (UID:0x%08X) with name \"%s\"\n", uid, name_param);

    return pdFALSE;
}
