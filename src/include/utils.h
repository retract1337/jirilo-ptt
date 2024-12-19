#ifndef UTILS_H
#define UTILS_H

#include "config.h"

void cleanup(int *mic_state, const struct BackendCommands *backend);
int command_exists(const char *cmd);

#endif// UTILS_H
