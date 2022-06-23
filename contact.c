#include "contact.h"

contact_t * create_contact(char * nick, struct sockaddr_in * addr) {
    
    contact_t * new_contact = malloc(sizeof(contact_t));
    if (check_null_error_stderr(new_contact, "Malloc of contact failed")) return NULL;

    new_contact->nick = strdup(nick);
    if (check_null_error_stderr(new_contact->nick, "malloc of contact nick failed")) {
        free(new_contact);
        return NULL;
    }

    new_contact->addr = malloc(sizeof(struct sockaddr));
    if (check_null_error_stderr(new_contact->addr, "malloc() of contact addr failed")) {
        free(new_contact->nick);
        free(new_contact);
        return NULL;
    }
    memcpy(new_contact->addr, addr, sizeof(struct sockaddr));

    new_contact->message_list = create_message_list();
    if (new_contact->message_list == NULL) {
        free(new_contact->nick);
        free(new_contact);
        return NULL;
    }
    new_contact->last_pkt_sent = 1;      // messages originating from us (first pkt will be sent as # 0)
    new_contact->last_ack_received = 1; // messages originating from us
    
    new_contact->last_pkt_received = -1; // we know that the first msg we will receive from a client has pkt # 0  

    new_contact->time_of_last_send = malloc(sizeof(struct timeval));
    if (check_null_error_stderr(new_contact->time_of_last_send, "malloc of contact time_of_last_send failed")) {
        free(new_contact->time_of_last_send);
        free(new_contact->nick);
        free(new_contact);
        return NULL;
    }

    new_contact->next = NULL;
    new_contact->prev = NULL;

    return new_contact;
}

void print_contact(void * data) {
    contact_t * contact = (contact_t *) data;
    char buffer [200];
    int port;
    int ret;
    ret = get_ip_and_port_from_sockaddr_in(buffer, &port, contact->addr);
    if (ret == 1) {
        printf("[%s, %s, %d]\n", contact->nick, buffer, port);
    } else {
        printf("[%s]\n", contact->nick);
    }
}

void free_contact(void * data) {
    contact_t * contact = (contact_t *) data;
    // printf("----------- freeing %s\n", contact->nick);
    free(contact->nick);
    free(contact->addr);
    free_message_list(contact->message_list);
    free(contact->time_of_last_send);
    free(contact);
    // printf("Contact was freed\n");
}

void print_contact_and_messages(contact_t * contact) {
    print_contact(contact);
    print_message_list(contact->message_list);
    printf("\n");
}

void add_message_to_contact_message_list(message_t * message, contact_t * contact) {
    add_message_to_list(contact->message_list, message);
}

int next_message_number_contact(contact_t * contact) {
    if (contact->last_ack_received == 0) return 1;
    if (contact->last_ack_received == 1) return 0;
    
    fprintf(stderr, "next_message_number failed because last_ack_received was %d", contact->last_ack_received);
    exit(EXIT_FAILURE);
}

void toggle_last_pkt_sent(contact_t * contact) {
    if (contact->last_pkt_sent == 0) contact->last_pkt_sent = 1;
    else contact->last_pkt_sent = 0;
}

int is_same_contact(void * data, void * otherData) {
    contact_t * firstContact = (contact_t *) data;
    contact_t * secondContact = (contact_t *) otherData;
    if (firstContact == secondContact) return 1;
    return 0;
}