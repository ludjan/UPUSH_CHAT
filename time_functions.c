#include "time_functions.h"

void set_time(struct timeval * time, long sec, long usec) {
    time->tv_sec = sec;
    time->tv_usec = usec;
}

void print_time(struct timeval * time) {
    printf("Sec %ld, usec %ld\n", time->tv_sec, time->tv_usec);
}

int compare_time(struct timeval * first_time, struct timeval * second_time) {
    if (first_time->tv_sec > second_time->tv_sec) return 1;
    if (first_time->tv_sec < second_time->tv_sec) return -1;
    if (first_time->tv_usec > second_time->tv_usec) return 1;
    if (first_time->tv_usec < second_time->tv_usec) return -1;
    else return 0; 
}

void set_time_minus_time(struct timeval * difference, struct timeval * first_time, struct timeval * second_time) {
    difference->tv_sec = first_time->tv_sec - second_time->tv_sec;
    difference->tv_usec = (first_time->tv_usec - second_time->tv_usec);
    if (first_time->tv_usec < second_time->tv_usec) {
        difference->tv_sec--;
        difference->tv_usec += 1000000;
    } 

}

void set_time_difference(struct timeval * difference, struct timeval * first_time, struct timeval * second_time) {
    if (compare_time(first_time, second_time) == 1) {
        set_time_minus_time(difference, first_time, second_time);
    } else {
        set_time_minus_time(difference, second_time, first_time);
    }
}

int set_smallest_time(struct timeval * time_buf, struct timeval * first_time, struct timeval * second_time) {
    if (compare_time(first_time, second_time) < 0) {
        set_time(time_buf, first_time->tv_sec, first_time->tv_usec);
        return 1;
    } else {
        set_time(time_buf, second_time->tv_sec, second_time->tv_usec);
        return 2;
    }
}