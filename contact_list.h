#ifndef CONTACT_LIST
#define CONTACT_LIST

#include "contact.h"
#include "base_linked_list.h"
#include "common.h"

base_linked_list_t * contact_list;

// gets a fresh base_linked_list_t to global variable
int init_contact_list();

// adds a contact to contact_list
int add_contact_to_contact_list(contact_t * contact);

// returns size of contact_list
int contact_list_size();

// frees contact list
void free_contact_list();

// prints contact list
void print_contact_list();

// remove contact closest to list tail
contact_t * pop_contact();

// remove a contact_t from the list structure without removing actual contact
void dettach_contact_from_contact_list(contact_t * contact);

// get a pointer to a contact that matches a nick
contact_t * find_contact_by_nick(char * nick);

// get a pointer to a contact that matches an address 
// (used for handling ACKs from other clients)
contact_t * find_contact_by_addr(struct sockaddr_in * addr);


/********* contact_iterator ****************/

typedef struct contact_iterator {
    base_iterator_t * iterator;
} contact_iterator_t;

contact_iterator_t * get_contact_iterator();

contact_t * contact_iterator_next (contact_iterator_t * contact_iterator);

int contact_iterator_has_next (contact_iterator_t * contact_iterator);

void free_contact_iterator(contact_iterator_t * contact_iterator);

#endif