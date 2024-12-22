#ifndef DIRECTORY_OPS_H
#define DIRECTORY_OPS_H

int list_directory(const char *path);
int create_file_or_directory(const char *path);
int search_file(const char *path, const char *file_name);
int remove_file_or_directory(const char *path);

#endif