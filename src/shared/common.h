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

#if COMPILER == COMPILER_INTEL
#include <ext/hash_map>
#elif COMPILER == COMPILER_GNU && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#include <tr1/unordered_map>
#elif COMPILER == COMPILER_GNU && __GNUC__ >= 3
#include <ext/hash_map>
#endif

#ifdef COMPILER == COMPILER_GNU && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#define UNORDERED_MAP std::tr1::unordered_map
#elif (COMPILER == COMPILER_GNU && __GNUC__ >= 3) || COMPILER == COMPILER_INTEL
#define UNORDERED_MAP __gnu_cxx::hash_map
namespace __gnu_cxx
{
    template<> struct hash<unsigned long long>
    {
        size_t operator()(const unsigned long long &__x) const { return (size_t)__x; }
    };
    template<typename T> struct hash<T *>
    {
        size_t operator()(T * const &__x) const { return (size_t)__x; }
    };
    template<> struct hash<std::string>
    {
        size_t operator()(const std::string &__x) const
        {
            return hash<const char *>()(__x.c_str());
        }
    };
};
#else
#define UNORDERED_MAP std::hash_map
using std::hash_map;
#endif

#include "utility/file.h"
#include "utility/logger.h"
#include "utility/timer.h"
#include "utility/config.h"
#include "exception.h"
//#include "revision.h"

#define uint32 uint32_t
#define uint16 uint16_t
#define uint8  uint8_t

typedef bool(*handler)(void *);

void   welcome();
int    exec_command(const char *fmt, ...);

#endif

