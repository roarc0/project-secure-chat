#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <string>

using namespace std;

int  file_size(const char *);
bool file_exists (const char *);
bool dir_exists(string dirname);
int  is_dir(const char *);
bool is_dir_empty(const char *);
void add_slash(string&);
void deltree(const char *);
char *list_files(const char *);

void   info_file(const char *,int ,const char *);
char*  read_file(const char *filename);

#endif
