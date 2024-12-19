#include "include/config.h"
#include "include/logger.h"

#include <stdio.h>
#include <stdlib.h>

void detect_backend(struct BackendCommands *backend) {
    if (system("which amixer > /dev/null 2>&1") == 0) {
        backend->enable = "amixer set Capture cap > /dev/null 2>&1";
        backend->disable = "amixer set Capture nocap > /dev/null 2>&1";
        backend->status = "amixer get Capture";

        LOG_INFO("[%s] Using ALSA backend", NAME);
    } else if (system("which pactl > /dev/null 2>&1") == 0) {
        backend->enable = "pactl set-source-mute @DEFAULT_SOURCE@ 0";
        backend->disable = "pactl set-source-mute @DEFAULT_SOURCE@ 1";
        backend->status = "pactl list sources";

        LOG_INFO("[%s] Using PulseAudio backend", NAME);
    } else {
        LOG_ERROR("[%s] No supported audio backend detected", NAME);
        exit(1);
    }
}

void set_state(int state, int *global_state, const struct BackendCommands *backend) {
    if (state == *global_state) {
        LOG_INFO("[%s] Microphone is already %s", NAME, state == MIC_ON ? "enabled" : "disabled");
        return;
    }

    if (state) {
        if (system(backend->enable) == 0) {
            LOG_INFO("[%s] Microphone enabled", NAME);
            *global_state = MIC_ON;
        } else {
            LOG_ERROR("[%s] Failed to enable microphone", NAME);
        }
    } else {
        if (system(backend->disable) == 0) {
            LOG_INFO("[%s] Microphone disabled", NAME);
            *global_state = MIC_OFF;
        } else {
            LOG_ERROR("[%s] Failed to disable microphone", NAME);
        }
    }
}

void get_status(const struct BackendCommands *backend) {
    char command[128];
    snprintf(command, sizeof(command), "%s | grep '\\[on\\]' > /dev/null 2>&1", backend->status);

    if (system(command) == 0) {
        LOG_INFO("[%s] Microphone is currently enabled", NAME);
    } else {
        LOG_INFO("[%s] Microphone is currently disabled", NAME);
    }

}
