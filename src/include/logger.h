#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifdef DEBUG
    #define LOG_ERROR(format, ...) fprintf(stderr, "[ERROR] " format "\n", ##__VA_ARGS__)
    #define LOG_WARNING(format, ...) fprintf(stderr, "[WARNING] " format "\n", ##__VA_ARGS__)
    #define LOG_INFO(format, ...) fprintf(stdout, "[INFO] " format "\n", ##__VA_ARGS__)
    #define LOG_DEBUG(format, ...) fprintf(stdout, "[DEBUG] " format "\n", ##__VA_ARGS__)
#else
    #define LOG_ERROR(format, ...) (void)0  
    #define LOG_WARNING(format, ...) (void)0  
    #define LOG_INFO(format, ...) (void)0  
    #define LOG_DEBUG(format, ...) (void)0 
#endif

#endif // LOGGER_H

