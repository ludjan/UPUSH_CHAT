#ifndef SERVER_CLIENT_LIST
#define SERVER_CLIENT_LIST

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#include "server_client.h"
#include "base_linked_list.h"

base_linked_list_t * client_list;

// get a new base_linked_list_t and put into global variable client_list
// returns 1 on success, -1 on fail
int init_server_client_list();

// add a client to the list
int add_server_client_to_list(server_client_t * client);

// get the size of the client list
int client_list_size();

// free the whole client list
void free_client_list();

// print the client list
void print_client_list();

// pop a server client from the list, and return a pointer to it
server_client_t * pop_client();

// dettach a client from the list, but don't delete it
void dettach_client_from_list(server_client_t * middle);

// try to find a client which matches the nick supplied
server_client_t * find_client_by_nick(char * nick);

#endif