#ifndef SERVER_CLIENT
#define SERVER_CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct server_client {
    char * nick;
    char * ip_address;
    int port;
    struct timeval last_heartbeat;
    struct server_client * next;
    struct server_client * prev;
    
} server_client_t;

// malloc a client and set fields
server_client_t * create_client(char * nick, char * ip, int port);

void print_client(void * data);

void free_client(void * data);

// returns 1 on success and -1 on fail
int update_client_nick(server_client_t * client, char * new_nick);

// returns 1 on success and -1 on fail
int update_client_ip_address(server_client_t * client, char * new_ip);

void update_client_port(server_client_t * client, int new_port);

// returns 1 on success and -1 on fail
int update_client_last_heartbeat(server_client_t * client);

int get_seconds_since_last_heartbeat(server_client_t * client);

int server_client_has_same_nick(void * data, void * otherData);

#endif