#ifndef _TAD_TIMER_H_
#define _TAD_TIMER_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

typedef struct timer Timer;

Timer *new_timer();
void delete_timer(Timer **t);
void start_timer(Timer *t);
void stop_timer(Timer *t);
void reset_timer(Timer *t);
float get_timer_result(Timer *t);

#endif