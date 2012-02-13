#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include "../common.h"

typedef struct timer
{
    struct timeval start;
    struct timeval stop;
} timer;

double timeval_diff(struct timeval *, struct timeval *); // deprecated

void   time_start(timer*);
void   time_stop(timer*);
double time_diff(timer*);

void   print_time_format(float sec);

int    msleep(unsigned long milisec);

#endif
