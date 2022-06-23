

#ifndef BLOCKED_NICK
#define BLOCKED_NICK

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

typedef struct blocked_nick {
    char * nick;
} blocked_nick_t;

blocked_nick_t * create_blocked_nick(char * nick);

/* functions for passing into ll_base_linked_list */
void free_blocked_nick(void * data);

int match_blocked_nick(void * data, void * other_data);

void print_blocked_nick(void * data);

#endif