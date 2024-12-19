#ifndef HANDLERS_H
#define HANDLERS_H

#include <time.h>
#include "config.h"

extern struct timespec last_signal_time; 

void state_handler(int state, int *mic_state, struct BackendCommands *backend);
void status_handler(struct BackendCommands *backend);
void ptt_handler(int *ptt_pressed, int *mic_state, struct BackendCommands *backend);

#endif //HANDLERS_H
