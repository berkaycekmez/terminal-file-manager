#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include "permissions.h"
#include "logger.h"

/*
 * Changes file permissions
 * @param path: Path to the file
 * @param mode_str: Permission string in octal (e.g., "755")
 * @return: 0 on success, -1 on failure
 */
int change_file_permissions(const char *path, const char *mode_str) {
    char *endptr;
    long mode = strtol(mode_str, &endptr, 8);

    // Validate permission format
    if (*endptr != '\0' || mode < 0 || mode > 0777 || mode == LONG_MAX || mode == LONG_MIN) {
        printf("Invalid permission format! Example: 755\n");
        return -1;
    }

    // Check if file exists
    if (access(path, F_OK) == -1) {
        perror("File not found");
        return -1;
    }

    log_operation("CHANGE_PERM", path, "STARTED");

    if (chmod(path, mode) == -1) {
        perror("Error changing permissions");
        log_operation("CHANGE_PERM", path, "ERROR");
        return -1;
    }

    printf("Changed permissions of %s to %s\n", path, mode_str);
    log_operation("CHANGE_PERM", path, "COMPLETED");
    return 0;
}