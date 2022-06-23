#ifndef BLOCKED_NICK_LIST
#define BLOCKED_NICK_LIST

#include "common.h"
#include "base_linked_list.h"
#include "blocked_nick.h"
#include "upush_client.h"

base_linked_list_t * blocked_list;

// gets a fresh base_linked_list_t to global variable
int init_blocked_nick_list();

// checks if nick alreay exists,
// if already exists, returns 0
// if does not exist, add it and return 1
// returns -1 on error
int add_blocked_nick(char * nick);

// checks if nick already exists,
// if it does, dettach from list, free and return 1
// if it does not, return 0
int remove_blocked_nick(char * nick);

// frees blocked_nick_list
void free_blocked_nick_list();

// prints blocked_nick_list
void print_blocked_nick_list();

// checks if nick is on blocked list
// returns 1 for yes
// returns 0 for no
int is_nick_blocked(char * nick);

// returns size of blocked_list
int blocked_nick_list_size();

// returns a pointer to a blocked nick 
// which has matching nick
blocked_nick_t * get_blocked_nick_by_nick(char * nick);

#endif