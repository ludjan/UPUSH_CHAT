#ifndef MESSAGE
#define MESSAGE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common.h"

typedef struct message {
    char * payload; // the actual message
    int attempts; // how many times we attempted to send the message
} message_t;

// create a message containing the payload
message_t * create_message(char * payload);

// print the message
void print_message(void * data);

// +1 to the attempt counter
void add_message_send_attempt(message_t * message);

// value of the attempt counter
int get_message_attempts(message_t * message);

// free the message and it's contents
void free_message(void * data);

#endif