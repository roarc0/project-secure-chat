#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "typedefs.h"
#include "utility/file.h"
#include "utility/logger.h"
#include "config.h"

#ifndef COMMON_H
#define COMMON_H

void   welcome();
int    exec_command(const char *fmt, ...);

#endif

