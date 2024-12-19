#include "include/handlers.h"
#include "include/logger.h"

struct timespec last_signal_time; 

void state_handler(int state, int *mic_state, struct BackendCommands *backend) {
    set_state(state, mic_state, backend);
    clock_gettime(CLOCK_MONOTONIC, &last_signal_time);
}

void status_handler(struct BackendCommands *backend) {
    get_status(backend);
    LOG_INFO("[%s] Status requested", NAME);
}

void ptt_handler(int *ptt_pressed, int *mic_state, struct BackendCommands *backend) {
    if (*ptt_pressed == PTT_RELEASED) {
        return;
    }

    *ptt_pressed = PTT_PRESSED;
    clock_gettime(CLOCK_MONOTONIC, &last_signal_time);

    if (*mic_state == MIC_ON) {
        set_state(MIC_OFF, mic_state, backend);
    }

    LOG_INFO("[%s] PTT pressed", NAME);
}
