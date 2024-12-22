#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "directory_ops.h"
#include "logger.h"

/*
 * Lists the contents of a directory
 * @param path: Directory path to list
 * @return: 0 on success, -1 on failure
 */
int list_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    log_operation("LIST_DIR", path, "STARTED");

    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        log_operation("LIST_DIR", path, "ERROR");
        return -1;
    }

    printf("Contents of directory %s:\n", path);
    printf("Name\t\tSize\t\tType\n");
    printf("------------------------------------\n");
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files and parent directory references
        if (entry->d_name[0] == '.') continue;
        
        // Allocate memory for full path
        char *full_path = malloc(strlen(path) + strlen(entry->d_name) + 2);
        if (full_path == NULL) {
            perror("Memory allocation error");
            log_operation("LIST_DIR", path, "ERROR");
            continue;
        }
        
        sprintf(full_path, "%s/%s", path, entry->d_name);
        if (stat(full_path, &file_stat) == -1) {
            perror("Could not get file info");
            free(full_path);
            continue;
        }

        printf("%-15s\t%ld bytes\t%s\n",
                entry->d_name,
                file_stat.st_size,
                S_ISDIR(file_stat.st_mode) ? "Directory" : "File");
        free(full_path);
    }

    closedir(dir);
    log_operation("LIST_DIR", path, "COMPLETED");
    return 0;
}

/*
 * Creates a new file or directory based on path
 * @param path: Path to create file/directory
 * @return: 0 on success, -1 on failure
 * Note: If path ends with '/', creates directory, otherwise creates file
 */
int create_file_or_directory(const char *path) {
    log_operation("CREATE", path, "STARTED");
    
    size_t len = strlen(path);
    if (len == 0) {
        printf("Invalid path!\n");
        log_operation("CREATE", path, "ERROR");
        return -1;
    }
    
    // Check if path ends with '/' to determine if it's a directory
    int is_directory = (path[len - 1] == '/');
    
    if (is_directory) {
        char *temp_path = strdup(path);
        if (temp_path == NULL) {
            perror("Memory allocation error");
            log_operation("CREATE", path, "ERROR");
            return -1;
        }
        
        // Remove trailing '/'
        temp_path[len - 1] = '\0';
        
        // Create directory with 755 permissions
        if (mkdir(temp_path, 0755) == -1) {
            if (errno != EEXIST) {
                perror("Error creating directory");
                free(temp_path);
                log_operation("CREATE", path, "ERROR");
                return -1;
            }
        } else {
            printf("Directory %s created.\n", temp_path);
        }
        
        free(temp_path);
        log_operation("CREATE", path, "COMPLETED");
        return 0;
    } else {
        // Create file with 644 permissions
        int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0644);
        if (fd == -1) {
            if (errno == EEXIST) {
                printf("File already exists: %s\n", path);
            } else {
                perror("Error creating file");
            }
            log_operation("CREATE", path, "ERROR");
            return -1;
        }

        printf("File %s created.\n", path);
        close(fd);
        
        log_operation("CREATE", path, "COMPLETED");
        return 0;
    }
}

/*
 * Searches for a file in the specified directory
 * @param path: Directory to search in
 * @param file_name: Name of file to search for
 * @return: 0 on success, -1 on failure
 */
int search_file(const char *path, const char *file_name) {
    DIR *dir;
    struct dirent *entry;

    log_operation("SEARCH_FILE", path, "STARTED");

    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        log_operation("SEARCH_FILE", path, "ERROR");
        return -1;
    }

    int found = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, file_name) == 0) {
            printf("File %s found in directory %s\n", file_name, path);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("File %s not found in directory %s\n", file_name, path);
    }

    closedir(dir);
    log_operation("SEARCH_FILE", path, "COMPLETED");
    return 0;
}

/*
 * Removes a file or directory
 * @param path: Path to file/directory to remove
 * @return: 0 on success, -1 on failure
 */
int remove_file_or_directory(const char *path) {
    log_operation("REMOVE", path, "STARTED");

    // Try removing as directory first
    if (rmdir(path) == 0) {
        printf("Directory %s removed.\n", path);
        log_operation("REMOVE", path, "COMPLETED");
        return 0;
    }

    // If directory is not empty, ask user
    if (errno == ENOTEMPTY) {
        char answer[10];
        printf("Directory %s is not empty. Remove anyway? (yes/no): ", path);
        scanf("%s", answer);

        if (strcmp(answer, "yes") == 0) {
            // Use system command to remove directory and contents
            char command[1024];
            sprintf(command, "find %s -delete", path);
            if (system(command) == 0) {
                printf("Directory %s and contents removed.\n", path);
                log_operation("REMOVE", path, "COMPLETED");
                return 0;
            } else {
                perror("Could not remove directory and contents");
                log_operation("REMOVE", path, "ERROR");
                return -1;
            }
        } else {
            printf("Remove operation cancelled.\n");
            log_operation("REMOVE", path, "CANCELED");
            return 0;
        }
    }

    // Try removing as file
    if (unlink(path) == 0) {
        printf("File %s removed.\n", path);
        log_operation("REMOVE", path, "COMPLETED");
        return 0;
    }

    perror("Remove operation failed");
    log_operation("REMOVE", path, "ERROR");
    return -1;
}