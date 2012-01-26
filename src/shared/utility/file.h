#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <dirent.h>

#include "../common.h"

int  file_size(const char *);
bool file_exists (const char *);
int  is_dir(const char *);
bool is_dir_empty(const char *);
void add_slash(char *);
void deltree(const char *);
char *list_files(const char *);
//char *sha_digest(const char *);
void print_file_info(const char *,int ,const char *);

#endif
