#include "timer.h"

struct timer {
    clock_t start, end;
};

Timer *new_timer()
{
    Timer *t = (Timer*)calloc(1, sizeof(Timer));
    t->start = -1;
    t->end = -1;
    return t;
}

void delete_timer(Timer **t)
{
    free(*t);
    *t = NULL;
}

void start_timer(Timer *t)
{
    t->start = clock();
}

void stop_timer(Timer *t)
{
    t->end = clock();
}

void reset_timer(Timer *t)
{
    t->start = -1;
    t->end = -1;
}

float get_timer_result(Timer *t)
{
    if(t->start == -1) return -1;
    if(t->end == -1) return -1;
    float e = (float)(t->end - t->start);
    return (e / CLOCKS_PER_SEC);
}