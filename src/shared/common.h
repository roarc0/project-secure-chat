#ifndef  _COMMON_H_
#define  _COMMON_H_

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
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "defines.h"
#include "utility/file.h"
#include "utility/logger.h"
#include "utility/timer.h"
#include "config.h"
#include "../../config.h"

#if COMPILER == COMPILER_INTEL
#include <ext/hash_map>
#elif COMPILER == COMPILER_GNU && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#include <tr1/unordered_map>
#elif COMPILER == COMPILER_GNU && __GNUC__ >= 3
#include <ext/hash_map>
#endif

#define UNORDERED_MAP std::tr1::unordered_map

typedef bool(*handler)(void *);

void   welcome();
int    exec_command(const char *fmt, ...);

#endif

