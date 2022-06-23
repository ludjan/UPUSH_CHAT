#include "message.h"

message_t * create_message(char * payload) {
    
    message_t * new_message = malloc(sizeof(message_t));
    if (check_null_error_stderr(new_message, "malloc() of message failed")) {
        return NULL;
    }

    new_message->payload = strdup(payload);
    if (check_null_error_stderr(new_message->payload, "strdup of message failed")) {
        free(new_message);
        return NULL;
    }
    new_message->attempts = 0;

    return new_message;
}

void print_message(void * data) {
    message_t * message = (message_t *) data;
    printf("   [%s, %d]\n", message->payload, message->attempts);
}

void add_message_send_attempt(message_t * message) {
    message->attempts = message->attempts + 1;
}

int get_message_attempts(message_t * message) {
    return message->attempts;
}

void free_message(void * data) {
    message_t * message = (message_t *) data;
    free(message->payload);
    free(message);
}
