#ifndef LOGGER_H
#define LOGGER_H

void log_start();
void log_operation(const char *operation, const char *path, const char *status);
void log_end();

#endif