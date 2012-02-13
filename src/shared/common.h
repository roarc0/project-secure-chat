#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <exception>

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
#include "utility/time.h"
#include "config.h"
#include "../../config.h"

#define DEBUG printf ("* Line %d of file %s (function <%s>)\n",\
                      __LINE__, __FILE__, __func__)

#ifndef  COMMON_H
#define COMMON_H

void   welcome();
int    exec_command(const char *fmt, ...);

#endif

