#ifndef MESSAGE_LIST
#define MESSAGE_LIST

#include "base_linked_list.h"
#include "message.h"
#include "common.h"

// I didn't want to put a base_linked_list wrapper into contact, 
// so I made this struct to separate the two a bit
typedef struct message_list {
    base_linked_list_t * list;
} message_list_t;

// create a fresh base_linked_list, and let "list" field contain a new base_linked_list
message_list_t * create_message_list();

// add a message to the list
int add_message_to_list(message_list_t * list, message_t * message);

// get the size of the list
int message_list_size(message_list_t * list);

// print all messages in list
void print_message_list(message_list_t * list);

// return pointer to the message closest to tail, but don't remove it from list
message_t * peek_message(message_list_t * list);

// return pointer to the message closest to tail, and remove it from list
message_t * pop_message(message_list_t * list);

// free the message_list
void free_message_list(message_list_t * list);

#endif