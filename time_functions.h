#ifndef TIME_FUNCTIONS
#define TIME_FUNCTIONS

#include <stdio.h>
#include "sys/time.h"

void set_time(
    struct timeval * time, 
    long sec, 
    long usec
);

void print_time(struct timeval * time);

// compares two times, returns 1 if first is greater, -1 if second is greater and 0 if equal
int compare_time(struct timeval * first_time, struct timeval * second_time);

// takes first time minus second time and puts into difference
void set_time_minus_time(struct timeval * difference, struct timeval * first_time, struct timeval * second_time);

// takes the bigger of first and second time, and subtracts the smaller of them, and puts result into difference 
void set_time_difference(struct timeval * difference, struct timeval * first_time, struct timeval * second_time);

// puts the smallest of first and second time into time_buf
int set_smallest_time(struct timeval * time_buf, struct timeval * first_time, struct timeval * second_time);

#endif