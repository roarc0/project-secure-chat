#include "time.h"

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
    float sec_res;
    uint32 min, hours, hours_res, days, days_res, years, years_res;

    if ( sec != 0.0f )
        sec_res = fmod(sec, (float)60);

    if (sec < 60)
        printf("%f second(s)", sec);
    else if (sec < 3600)
    {
        min = (uint32)sec/60;
        printf("%d minute(s) %f second(s)", min, sec_res);
    }
    else if ( sec < 86400 )
    {
        hours = (uint32) sec/3600;
        hours_res = (uint32)sec%3600;
        min = (uint32)hours_res/60;
        printf("%d hour(s) %d minute(s) %f second(s)", hours, min, sec_res);
    }
    else if ( sec < 31536000 )
    {
        days = (uint32) sec/86400;
        days_res = (uint32)sec%86400;
        hours = (uint32) days_res/3600;
        hours_res = (uint32)(sec - 86400)%3600;
        min = (uint32)hours_res/60;
        printf("%d day(s) %d hour(s) %d minute(s) %f second(s)", days, hours, min, sec_res);
    }
    else
    {
        years = (uint32) sec/31536000;
        years_res = (uint32) sec%31536000;
        days = (uint32) years_res/86400;
        days_res = (uint32)sec%86400;
        hours = (uint32) days_res/3600;
        hours_res = (uint32)(sec - 86400)%3600;
        min = (uint32)hours_res/60;
        printf("%d years %d day(s) %d hour(s) %d minute(s) %f second(s)", years, days, hours, min, sec_res);
    }
    printf("\n");
}
