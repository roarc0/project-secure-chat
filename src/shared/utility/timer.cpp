#include "timer.h"
#include <cstdio>
#include <iomanip> // std::setw

double timeval_diff(struct timeval *end_time, struct timeval *start_time)
{
  struct timeval diff;

  diff.tv_sec =end_time->tv_sec -start_time->tv_sec ;
  diff.tv_usec=end_time->tv_usec-start_time->tv_usec;

  return (double)(diff.tv_sec+(double)diff.tv_usec/(double)CLOCKS_PER_SEC);
}

double time_diff(timer *time)
{ 
  struct timeval diff;

  diff.tv_sec  = time->stop.tv_sec - time->start.tv_sec ;
  diff.tv_usec = time->stop.tv_usec - time->start.tv_usec;

  return (double)(diff.tv_sec + (double)diff.tv_usec / (double)CLOCKS_PER_SEC);
}

void time_start(timer *time)
{
    gettimeofday(&time->start,NULL);
}

void time_stop(timer *time)
{
    gettimeofday(&time->stop,NULL);
}

void print_time_format(float sec)
{
    float sec_res = 0.0f;
    int min, hours, hours_res, days, days_res, years, years_res;

    if ( sec != 0.0f )
        sec_res = fmod(sec, (float)60);

    if (sec < 60)
        printf("%f second(s)", sec);
    else if (sec < 3600)
    {
        min = (int)sec/60;
        printf("%d minute(s) %f second(s)", min, sec_res);
    }
    else if ( sec < 86400 )
    {
        hours = (int) sec/3600;
        hours_res = (int)sec%3600;
        min = (int)hours_res/60;
        printf("%d hour(s) %d minute(s) %f second(s)", hours, min, sec_res);
    }
    else if ( sec < 31536000 )
    {
        days = (int) sec/86400;
        days_res = (int)sec%86400;
        hours = (int) days_res/3600;
        hours_res = (int)(sec - 86400)%3600;
        min = (int)hours_res/60;
        printf("%d day(s) %d hour(s) %d minute(s) %f second(s)", days, hours, min, sec_res);
    }
    else
    {
        years = (int) sec/31536000;
        years_res = (int) sec%31536000;
        days = (int) years_res/86400;
        days_res = (int)sec%86400;
        hours = (int) days_res/3600;
        hours_res = (int)(sec - 86400)%3600;
        min = (int)hours_res/60;
        printf("%d years %d day(s) %d hour(s) %d minute(s) %f second(s)", years, days, hours, min, sec_res);
    }
    printf("\n");
}

int __nsleep(const struct timespec *req, struct timespec *rem)
{
    struct timespec temp_rem;
    if (nanosleep(req,rem)==-1)
    {
        __nsleep(rem,&temp_rem);
        return 0;
    }
    else
        return 1;
}
 
int msleep(unsigned long milisec)
{
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    struct timespec req={0,0},rem={0,0};
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    __nsleep(&req,&rem);
    return 1;
}

std::string get_timestamp(char sep)
{
    stringstream ss;
    time_t epoch_time;
    struct tm *tm_p;

    epoch_time = time( NULL );
    tm_p = localtime( &epoch_time );

    if(tm_p->tm_hour < 10)
        ss << '0';
    ss << tm_p->tm_hour << sep;
    if(tm_p->tm_min < 10)
        ss << '0';
    ss << tm_p->tm_min << sep;
    if(tm_p->tm_sec < 10)
        ss << '0';
    ss << tm_p->tm_sec;

    return ss.str();
}
