#include <stdio.h>
#include <string.h>
#include "directory_ops.h"
#include "file_operations.h"
#include "permissions.h"
#include "logger.h"

/*
 * Displays help information about available commands
 */
void print_help() {
    printf("Terminal File Manager Usage Guide:\n");
    printf("  slist /path/to/dir           - List directory contents\n");
    printf("  screate /path/to/file        - Create file/directory\n");
    printf("  scopy source destination     - Copy file\n");
    printf("  smove source destination     - Move file\n");
    printf("  sremove /path/to/file        - Remove file/directory\n");
    printf("  sview /path/to/file          - View file contents\n");
    printf("  ssearch /path/to/dir filename - Search for file\n");
    printf("  spermission /path/to/file 755 - Change file permissions\n");
    printf("  exit                         - Exit program\n");
}

/*
 * Main function - Entry point of the program
 * Handles command line arguments and calls appropriate functions
 */
int main(int argc, char *argv[]) {
    log_start();

    if (argc < 2) {
        print_help();
        log_end();
        return 1;
    }

    int result = -1;
    
    // Command parsing and execution
    if (strcmp(argv[1], "slist") == 0 && argc == 3) {
        result = list_directory(argv[2]);
    } else if (strcmp(argv[1], "screate") == 0 && argc == 3) {
        result = create_file_or_directory(argv[2]);
    } else if (strcmp(argv[1], "scopy") == 0 && argc == 4) {
        result = copy_file(argv[2], argv[3]);
    } else if (strcmp(argv[1], "smove") == 0 && argc == 4) {
        result = move_file(argv[2], argv[3]);
    } else if (strcmp(argv[1], "sremove") == 0 && argc == 3) {
        result = remove_file_or_directory(argv[2]);
    } else if (strcmp(argv[1], "sview") == 0 && argc == 3) {
        result = view_file_contents(argv[2]);
    } else if (strcmp(argv[1], "ssearch") == 0 && argc == 4) {
        result = search_file(argv[2], argv[3]);
    } else if (strcmp(argv[1], "spermission") == 0 && argc == 4) {
        result = change_file_permissions(argv[2], argv[3]);
    } else if (strcmp(argv[1], "exit") == 0) {
        printf("Exiting program...\n");
        log_end();
        return 0;
    } else {
        printf("Invalid command! Usage:\n");
        print_help();
        log_end();
        return 1;
    }

    log_end();
    return result == 0 ? 0 : 1;
}