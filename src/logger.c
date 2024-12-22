#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"

FILE *log_file = NULL;

/*
 * Initializes the logging system
 * Opens the log file in append mode
 */
void log_start() {
    log_file = fopen("file_manager.log", "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(1);
    }
}

/*
 * Logs an operation with timestamp
 * @param operation: Type of operation
 * @param path: File/directory path
 * @param status: Operation status
 */
void log_operation(const char *operation, const char *path, const char *status) {
    if (log_file != NULL) {
        time_t now;
        time(&now);
        char *time_str = ctime(&now);
        time_str[strcspn(time_str, "\n")] = 0;  // Remove newline

        fprintf(log_file, "[%s] Operation: %s, Path: %s, Status: %s\n", 
                time_str, operation, path, status);
        fflush(log_file);
    }
}

/*
 * Closes the logging system
 * Closes the log file handle
 */
void log_end() {
    if (log_file != NULL) {
        fclose(log_file);
    }
}