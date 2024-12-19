#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#define LOG_ERROR(format, ...) fprintf(stderr, "[ERROR] " format "\n", ##__VA_ARGS__)
#define LOG_WARNING(format, ...) fprintf(stderr, "[WARNING] " format "\n", ##__VA_ARGS__)
#define LOG_INFO(format, ...) fprintf(stdout, "[INFO] " format "\n", ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) fprintf(stdout, "[DEBUG] " format "\n", ##__VA_ARGS__)

#endif // LOGGER_H

