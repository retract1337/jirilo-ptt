#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "include/config.h"
#include "include/handlers.h"
#include "include/logger.h"
#include "include/utils.h"

typedef void (*CommandHandler)(int, int *, struct BackendCommands *);

typedef struct {
    const char *command;
    CommandHandler handler;
    int param;
} CommandMap;

static struct BackendCommands backend;
static int mic_state = MIC_OFF;
static char *callback[512] = {0};
static struct timespec current_time;

static CommandMap command_map[] = {
        {"1", state_handler, MIC_ON},
        {"0", state_handler, MIC_OFF},
        {"status", (CommandHandler) status_handler, 0},
        {"ptt", (CommandHandler) ptt_handler, 0},
        {NULL, NULL, 0}};

static void print_help() {
    printf("Usage: %s [-t timeout] [-i interval] [-c command] [-h]\n", NAME);
    printf("\t-t timeout\t Timeout in ms for detecting signal\n");
    printf("\t-i interval\t Check interval in ms\n");
    printf("\t-c command\t Command to execute on timeout\n");
    printf("\t-h\t\t Print this help\n");
}

static void signal_handler(int signal) {
    LOG_INFO("[%s] Received signal %d, cleaning up...", NAME, signal);

    cleanup(&mic_state, &backend);
    exit(EXIT_SUCCESS);
}

void timeout_handler(int timeout) {
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    long elapsed_sec = current_time.tv_sec - last_signal_time.tv_sec;
    long elapsed_nsec = current_time.tv_nsec - last_signal_time.tv_nsec;
    long elapsed_ms = elapsed_sec * 1000 + elapsed_nsec / 1000000L;

    if (elapsed_ms >= timeout && mic_state) {
        LOG_INFO("[%s] No signal detected for %d ms, disabling microphone...\n", NAME, timeout);
        set_state(MIC_OFF, &mic_state, &backend);

        if (*callback != NULL) {
            system(*callback);
        }
    }
}

static void process_command(const char *input) {
    char *token = strdup(input);
    char *strtok_result = strtok(token, "\n");

    while (strtok_result != NULL) {
        int i;

        for (i = 0; command_map[i].command != NULL; i++) {
            if (strcmp(strtok_result, command_map[i].command) == 0) {
                command_map[i].handler(command_map[i].param, &mic_state, &backend);
                break;
            }
        }

        if (command_map[i].command == NULL) {
            LOG_ERROR("[%s] Unknown command: %s", NAME, strtok_result);
        }

        strtok_result = strtok(NULL, "\n");
    }
    free(token);
}

int main(int argc, char *argv[]) {
    int opt;

    int custom_check_interval = DEFAULT_CHECK_INTERVAL;
    int custom_timeout = SIGNAL_TIMEOUT;

    unlink(FIFO_PATH);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while ((opt = getopt(argc, argv, "t:i:c:h")) != -1) {
        switch (opt) {
            case 't':
                custom_timeout = strtol(optarg, NULL, 10);
                if (custom_timeout == 0) {
                    LOG_ERROR("[%s] Invalid timeout value: %s", NAME, optarg);
                    print_help();
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                custom_check_interval = strtol(optarg, NULL, 10);
                if (custom_check_interval == 0) {
                    LOG_ERROR("[%s] Invalid interval value: %s", NAME, optarg);
                    print_help();
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                callback[0] = strdup(optarg);
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

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

        timeout_handler(custom_timeout);
        usleep(custom_check_interval * 1000);
    }

    close(fd);
    unlink(FIFO_PATH);

    return EXIT_SUCCESS;
}

