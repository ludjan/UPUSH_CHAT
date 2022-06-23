#ifndef CLIENT_SERVER
#define CLIENT_SERVER

#include "common.h"

typedef struct client_server {
    struct sockaddr_in * addr;
    int last_ack_received;
} client_server_t;

client_server_t * server;

// mallocs and initiates global variable "server"
int init_client_server();

// print server, for testing
void print_client_server();

// free allocated memory
void free_client_server();

// find which next pkt number to server should be
int next_message_number_server();

#endif