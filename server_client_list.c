
#include "server_client_list.h"

int init_server_client_list() {
    client_list = ll_create_base_linked_list();
    if (client_list == NULL) return -1;
    return 1;
}

int add_server_client_to_list(server_client_t * new_client) {
    return ll_add_node(client_list, new_client);
}

void free_client_list() {
    ll_free_linked_list(client_list, free_client);
}

int size_client_list() {
    return ll_base_list_size(client_list);
}

void print_client_list() {
    ll_do_on_each(client_list, print_client);
}

void dettach_client_from_list(server_client_t * client) {
    ll_pop_on_match(client_list, server_client_has_same_nick, client->nick);
}

server_client_t * pop_client() {
    server_client_t * tmp = (server_client_t *) ll_pop_node(client_list);
    return tmp;
}

server_client_t * find_client_by_nick(char * nick) {
    server_client_t * tmp = (server_client_t *) ll_find_by_matching_data(client_list, server_client_has_same_nick, nick);
    return tmp;
}