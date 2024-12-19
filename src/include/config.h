#ifndef CONFIG_H
#define CONFIG_H

#define NAME "Jirilo PTT"
#define FIFO_PATH "/tmp/jirilo_ptt"
#define SIGNAL_TIMEOUT 30 // timeout in ms
#define DEFAULT_CHECK_INTERVAL 100 // check interval in ms

#define MIC_ON 1
#define MIC_OFF 0

#define PTT_PRESSED 1
#define PTT_RELEASED 0

struct BackendCommands {
    const char *enable;
    const char *disable;
    const char *status;
};

void detect_backend(struct BackendCommands *backend);
void set_state(int state, const struct BackendCommands *backend);
void get_status(const struct BackendCommands *backend);

#endif // CONFIG_H
