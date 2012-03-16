#ifndef TIMER_H
#define TIMER_H

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

struct IntervalTimer
{
    public:

        IntervalTimer()
            : _interval(0), _current(0)
        {
        }

        void Update(time_t diff)
        {
            _current += diff;
            if (_current < 0)
                _current = 0;
        }

        bool Passed()
        {
            return _current >= _interval;
        }

        void Reset()
        {
            if (_current >= _interval)
                _current -= _interval;
        }

        void SetCurrent(time_t current)
        {
            _current = current;
        }

        void SetInterval(time_t interval)
        {
            _interval = interval;
        }

        time_t GetInterval() const
        {
            return _interval;
        }

        time_t GetCurrent() const
        {
            return _current;
        }

    private:

        time_t _interval;
        time_t _current;
};

#endif
