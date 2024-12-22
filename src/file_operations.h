#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

int copy_file(const char *source, const char *destination);
int move_file(const char *source, const char *destination);
int view_file_contents(const char *path);

#endif