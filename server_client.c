#include "server_client.h"

server_client_t * create_client(char * nick, char * ip, int port) {
    
    int ret;

    server_client_t * new_client = malloc(sizeof(server_client_t));
    if (check_null_error_stderr(new_client, "malloc of client failed")) return NULL;

    new_client->nick = strdup(nick);
    if (check_null_error_stderr(new_client->nick, "strdup of client nick failed")) {
        free(new_client);
        return NULL;
    }
    
    new_client->ip_address = strdup(ip);
    if (check_null_error_stderr(new_client->nick, "strdup of client ip address failed")) {
        free(new_client->nick);
        free(new_client);
        return NULL;
    }

    ret = gettimeofday(&new_client->last_heartbeat, NULL);
    if (check_error_perror(ret, "gettimeofday")) {
        free(new_client->ip_address);
        free(new_client->nick);
        free(new_client);
        return NULL;
    }
    
    new_client->port = port;
    
    new_client->next = NULL;
    new_client->prev = NULL;

    return new_client;
}

void print_client(void * data) {
    server_client_t * client = (server_client_t *) data;
    printf("[%s, %s, %d, %ld]\n", client->nick, client->ip_address, client->port, client->last_heartbeat.tv_sec);
}

int update_client_nick(server_client_t * client, char * new_nick) {
    free(client->nick);
    client->nick = strdup(new_nick);
    if (check_null_error_stderr(client->nick, "strdup in update_client_nick")) return -1;
    return 1;
}

int update_client_ip_address(server_client_t * client, char * new_ip) {
    free(client->ip_address);
    client->ip_address = strdup(new_ip);
    if (check_null_error_stderr(client->ip_address, "strdup in update_client_nick")) return -1;
    return 1;
}

void update_client_port(server_client_t * client, int new_port) {
    client->port = new_port;
}

int update_client_last_heartbeat(server_client_t * client) {

    int ret;
    struct timeval current_time;
    
    ret = gettimeofday(&current_time, NULL);
    if (check_error_perror(ret, "gettimeofday")) return -1;

    memcpy(&client->last_heartbeat, &current_time, sizeof(struct timeval));
    return 1;

}

int get_seconds_since_last_heartbeat(server_client_t * client) {
    struct timeval current_time;
    
    if ( gettimeofday(&current_time, NULL) == -1) {
        perror("gettimeofday");
        return -1;
    }
    return current_time.tv_sec - client->last_heartbeat.tv_sec;
}

void free_client(void * data) {
    server_client_t * client = (server_client_t *) data;
    // printf("----------- freeing %s\n", client->nick);
    free(client->nick);
    free(client->ip_address);
    free(client);
}

int server_client_has_same_nick(void * data, void * otherData) {
    server_client_t * client = (server_client_t *) data;
    char * otherNick = (char *) otherData;
    if (strcmp(client->nick, otherNick) == 0) return 1;
    return 0;
}