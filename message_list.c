#include "message_list.h"

message_list_t * create_message_list() {
    
    message_list_t * msg_list = malloc(sizeof(message_list_t));
    if (check_null_error_stderr(msg_list, "malloc of message list failed")) {
        return NULL; 
    }
    msg_list->list = ll_create_base_linked_list();
    if (msg_list->list == NULL) {
        free(msg_list);
        return NULL;
    }
    
    return msg_list;
}

int add_message_to_list(message_list_t * msg_list, message_t * new_message) {
    return ll_add_node(msg_list->list, new_message);
}

void free_message_list(message_list_t * msg_list) {
    ll_free_linked_list(msg_list->list, free_message);
    free(msg_list);
}

int message_list_size(message_list_t * msg_list) {
    return ll_base_list_size(msg_list->list);
}

void print_message_list(message_list_t * msg_list) {
    ll_do_on_each(msg_list->list, print_message);
}

message_t * peek_message(message_list_t * msg_list) {
    message_t * tmp = (message_t *) ll_peek(msg_list->list);
    return tmp;
}

message_t * pop_message(message_list_t * msg_list) {
    message_t * tmp = (message_t *) ll_pop_node(msg_list->list);
    return tmp;
}