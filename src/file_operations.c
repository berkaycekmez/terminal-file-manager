#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "file_operations.h"
#include "logger.h"

/*
 * Copies a file from source to destination
 * @param source: Source file path
 * @param destination: Destination file path
 * @return: 0 on success, -1 on failure
 */
int copy_file(const char *source, const char *destination) {
    FILE *src, *dest;
    char buffer[4096];
    size_t bytes_read;

    log_operation("COPY_FILE", source, "STARTED");

    // Validate file paths
    if (source == NULL || destination == NULL || strlen(source) == 0 || strlen(destination) == 0) {
        printf("Invalid file path!\n");
        log_operation("COPY_FILE", source, "ERROR");
        return -1;
    }

    src = fopen(source, "rb");
    if (src == NULL) {
        perror("Error opening source file");
        log_operation("COPY_FILE", source, "ERROR");
        return -1;
    }

    dest = fopen(destination, "wb");
    if (dest == NULL) {
        perror("Error opening destination file");
        fclose(src);
        log_operation("COPY_FILE", destination, "ERROR");
        return -1;
    }

    // Copy file contents in chunks
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dest) != bytes_read) {
            perror("Error writing to file");
            fclose(src);
            fclose(dest);
            log_operation("COPY_FILE", destination, "ERROR");
            return -1;
        }
    }

    if (ferror(src)) {
        perror("Error reading from file");
        fclose(src);
        fclose(dest);
        log_operation("COPY_FILE", source, "ERROR");
        return -1;
    }

    fclose(src);
    fclose(dest);

    printf("File copied: %s -> %s\n", source, destination);
    log_operation("COPY_FILE", source, "COMPLETED");
    return 0;
}

/*
 * Moves a file from source to destination
 * @param source: Source file path
 * @param destination: Destination file path
 * @return: 0 on success, -1 on failure
 */
int move_file(const char *source, const char *destination) {
    struct stat dest_stat;
    log_operation("MOVE_FILE", source, "STARTED");

    // Validate file paths
    if (source == NULL || destination == NULL || strlen(source) == 0 || strlen(destination) == 0) {
        printf("Invalid file path!\n");
        log_operation("MOVE_FILE", source, "ERROR");
        return -1;
    }

    // Check if destination is a directory
    if (stat(destination, &dest_stat) == 0 && S_ISDIR(dest_stat.st_mode)) {
        char new_path[PATH_MAX];
        snprintf(new_path, PATH_MAX, "%s/%s", destination, basename((char *)source));

        if (rename(source, new_path) == 0) {
            printf("File moved: %s -> %s\n", source, new_path);
            log_operation("MOVE_FILE", source, "COMPLETED");
            return 0;
        } else {
            perror("Error moving file");
            log_operation("MOVE_FILE", source, "ERROR");
            return -1;
        }
    } else if (rename(source, destination) == 0) {
        printf("File moved: %s -> %s\n", source, destination);
        log_operation("MOVE_FILE", source, "COMPLETED");
        return 0;
    } else {
        perror("Error moving file");
        log_operation("MOVE_FILE", source, "ERROR");
        return -1;
    }
}

/*
 * Displays the contents of a file
 * @param path: Path to the file
 * @return: 0 on success, -1 on failure
 */
int view_file_contents(const char *path) {
    FILE *file;
    char buffer[1024];

    log_operation("VIEW_FILE", path, "STARTED");

    if (path == NULL || strlen(path) == 0) {
        printf("Invalid file path!\n");
        log_operation("VIEW_FILE", path, "ERROR");
        return -1;
    }

    file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        log_operation("VIEW_FILE", path, "ERROR");
        return -1;
    }

    printf("* Contents of %s *\n", path);
    printf("------------------------------------\n");
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    printf("\n------------------------------------\n");

    fclose(file);

    log_operation("VIEW_FILE", path, "COMPLETED");
    return 0;
}