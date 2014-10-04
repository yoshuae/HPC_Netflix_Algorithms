#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>
#include <time.h>
double t1,t2;
struct timeval tim;
void startTimer()
{
    gettimeofday(&tim, NULL);
    t1=tim.tv_sec+(tim.tv_usec/1000000.0);
}
double stopTimer()
{
    gettimeofday(&tim, NULL);
    double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
    return t2-t1;
}

#endif
