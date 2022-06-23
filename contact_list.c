#include "contact_list.h"

// mallocs a fresh blocked_nick_list_t to global variable
int init_contact_list() {
    contact_list = ll_create_base_linked_list();
    if (contact_list == NULL) return -1;
    return 1;
}

int add_contact_to_contact_list(contact_t * new_contact) {
    return ll_add_node(contact_list, new_contact);
}

// frees blocked_nick_list
void free_contact_list() {
    ll_free_linked_list(contact_list, free_contact);
}

// returns size of contact_list
int contact_list_size() {
    return ll_base_list_size(contact_list);
}

void print_contact_list() {
    ll_do_on_each(contact_list, print_contact);
}

contact_t * pop_contact() { 
    contact_t * tmp = (contact_t *) ll_pop_node(contact_list);
    return tmp;
}

int match_nick(void * data, void * other_data) {
    contact_t * contact = (contact_t *) data;
    char * other_nick = (char *) other_data;
    if (strcmp(contact->nick, other_nick) == 0) return 1;
    return 0;
}

contact_t * find_contact_by_nick(char * nick) {
    contact_t * tmp = (contact_t *) ll_find_by_matching_data(contact_list, match_nick, nick);
    return tmp; 
}

int compare_address(void * data, void * otherData) {
    contact_t * contact = (contact_t*) (data);
    struct sockaddr_in * other_addr = (struct sockaddr_in *) otherData;

    if (is_same_address(contact->addr, other_addr)) return 1;
    return 0;
}

contact_t * find_contact_by_addr(struct sockaddr_in * addr) {
    contact_t * tmp = (contact_t *) ll_find_by_matching_data(contact_list, compare_address, addr);
    return tmp;
}

void dettach_contact_from_contact_list(contact_t * contact) {
    ll_pop_on_match(contact_list, is_same_contact, contact);
}


/******* contact_iterator ********/

contact_iterator_t * get_contact_iterator() {
    contact_iterator_t * contact_iterator = malloc(sizeof(contact_iterator_t));
    if (check_null_error_stderr(contact_iterator, "malloc in for contact_iterator failed")) return NULL;
    
    contact_iterator->iterator = get_base_iterator(contact_list);
    if (contact_iterator->iterator == NULL) {
        free(contact_iterator);
        return NULL;
    }
    return contact_iterator;
}

contact_t * contact_iterator_next (contact_iterator_t * contact_iterator) {
    contact_t * tmp = (contact_t *) base_iterator_next(contact_iterator->iterator);
    return tmp;
}

int contact_iterator_has_next (contact_iterator_t * contact_iterator) {
    return base_iterator_has_next(contact_iterator->iterator);
}

void free_contact_iterator(contact_iterator_t * contact_iterator) {
    base_iterator_free(contact_iterator->iterator);
    free(contact_iterator);
}