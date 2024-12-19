#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

#include "include/config.h"
#include "include/logger.h"

typedef void (*CommandHandler)(void);

typedef struct {
    const char *command;
    CommandHandler handler;
} CommandMap;

static struct BackendCommands backend;
static int mic_state = MIC_OFF;
static int ptt_pressed = PTT_RELEASED;

static struct timespec current_time;
static struct timespec last_signal_time;

static void on_handler(void) {
    if (mic_state == MIC_ON) {
        return;
    }

    set_state(MIC_ON, &backend);
    mic_state = MIC_ON;

    clock_gettime(CLOCK_MONOTONIC, &last_signal_time);
    LOG_INFO("[%s] MIC turned ON", NAME);
}

static void off_handler(void) {
    if (mic_state == MIC_OFF) {
        return;
    }

    set_state(MIC_OFF, &backend);
    mic_state = MIC_OFF;

    clock_gettime(CLOCK_MONOTONIC, &last_signal_time); 
    LOG_INFO("[%s] MIC turned OFF", NAME);
}

static void status_handler(void) {
    get_status(&backend);
    LOG_INFO("[%s] Status requested", NAME);
}

static void ptt_handler(void) {
    if (ptt_pressed == PTT_RELEASED) {
        return;
    }

    ptt_pressed = 1;
    clock_gettime(CLOCK_MONOTONIC, &last_signal_time);

    if (mic_state == MIC_ON) {
        set_state(MIC_OFF, &backend);
        mic_state = MIC_OFF;
    }

    LOG_INFO("[%s] PTT pressed", NAME);
}

void timeout_handler() {
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    long elapsed_sec = current_time.tv_sec - last_signal_time.tv_sec;
    long elapsed_nsec = current_time.tv_nsec - last_signal_time.tv_nsec;
    long elapsed_ms = elapsed_sec * 1000 + elapsed_nsec / 1000000L;

    if (elapsed_ms >= SIGNAL_TIMEOUT && mic_state) {
        LOG_INFO("[%s] No signal detected for %d ms, disabling microphone...\n", NAME, SIGNAL_TIMEOUT);

        set_state(MIC_OFF, &backend);
        mic_state = MIC_OFF;
    }
}

static void exit_handler(void) {
    exit(0);
}

static CommandMap command_map[] = {
    { "1", on_handler },
    { "0", off_handler },
    { "status", status_handler },
    { "ptt", ptt_handler },
    { "exit", exit_handler },
    { NULL, NULL }
};

static void process_command(const char *input) {
    char *token = strtok(input, "\n");
    while (token != NULL) {
        int i;

        for (i = 0; command_map[i].command != NULL; i++) {
            if (strcmp(token, command_map[i].command) == 0) {
                command_map[i].handler();
                break;
            }
        }

        if (command_map[i].command == NULL) {
            LOG_ERROR("[%s] Unknown command: %s", NAME, token);
        }

        token = strtok(NULL, "\n");
    }
}

void cleanup() {
    set_state(MIC_OFF, &backend);
    unlink(FIFO_PATH);
    LOG_INFO("[%s] Cleanup performed", NAME);
}

int main(int argc, char *argv[]) {
    atexit(cleanup); 
    unlink(FIFO_PATH);  

    detect_backend(&backend);

    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Failed to create FIFO");
        return 1;
    }

    LOG_INFO("[%s] Waiting for commands via FIFO: %s", NAME, FIFO_PATH);

    int fd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Error opening FIFO");
        return 1;
    }

    char buffer[1024]; 

    while (1) {
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            process_command(buffer);
        }

        timeout_handler();
        usleep(DEFAULT_CHECK_INTERVAL * 1000);
    }

    close(fd);
    unlink(FIFO_PATH);

    return EXIT_SUCCESS;
}

