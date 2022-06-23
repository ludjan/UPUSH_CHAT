#include "client_server.h"

int init_client_server() {

    server = malloc(sizeof(client_server_t));
    if (check_null_error_stderr(server, "malloc of client server failed")) return -1;

    server->last_ack_received = 1;

    server->addr = malloc(sizeof(struct sockaddr));
    if (check_null_error_stderr(server->addr, "malloc of client server address failed")) {
        free(server);
        return -1;
    }

    return 1;
}

void print_client_server() {
    printf("[This is a server, it's last ack was %d]\n", server->last_ack_received);
}

void free_client_server() {
    free(server->addr);
    free(server);
}

int next_message_number_server() {
    if (server->last_ack_received == 0) return 1;
    else return 0;
}
