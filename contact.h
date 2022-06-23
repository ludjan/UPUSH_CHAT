#ifndef CONTACT
#define CONTACT


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_list.h"
#include "common.h"

typedef struct contact {
    char * nick;
    struct sockaddr_in * addr;
    struct message_list * message_list;
    int last_pkt_sent; // messages originating from us
    int last_ack_received; // messages originating from us
    int last_pkt_received; // messages originating from contact
    struct contact * next;
    struct contact * prev;
    struct timeval * time_of_last_send;
} contact_t;

// malloc a new contact and put paramaters into struct
contact_t * create_contact(char * nick, struct sockaddr_in * addr);

// free a contact
void free_contact(void * data);

// print a contact
void print_contact(void * data);

// update a contacts nickname
void update_contact_nick(contact_t * contact, char * new_nick);

// update a contacts address
void update_contact_address(contact_t * contact, struct sockaddr_in * addr);

// print a contact and it's message queue
void print_contact_and_messages(contact_t * contact);

// add a message to a contacts message queue
void add_message_to_contact_message_list(message_t * message, contact_t * contact);

// find which next message number should be for a contact
int next_message_number_contact(contact_t * contact);

// toggle value of last_pkt_sent between 0 and 1
void toggle_last_pkt_sent(contact_t * contact);

// compare two contacts, returns 1 if they are identical
int is_same_contact(void * data, void * otherData);

#endif