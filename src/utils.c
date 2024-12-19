#include "include/utils.h"
#include "include/logger.h"

#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>


void cleanup(int *mic_state, const struct BackendCommands *backend) {
    if (mic_state && backend) {
        set_state(MIC_OFF, mic_state, backend);
    } else {
        LOG_ERROR("[%s] Invalid state or backend in cleanup", NAME);
    }

    if (unlink(FIFO_PATH) == -1) {
        perror("Failed to unlink FIFO");
    }

    LOG_INFO("[%s] Cleanup performed", NAME);
}

int command_exists(const char *cmd) {
    char command[128];
    snprintf(command, sizeof(command), "which %s > /dev/null 2>&1", cmd);
    return (system(command) == 0);
}

