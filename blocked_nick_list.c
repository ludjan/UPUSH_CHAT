#include "blocked_nick_list.h"

/* functions are documented in header file */

/* the match_blocked_nick and free_blocked_nick are in blocked_nick.h*/ 

int init_blocked_nick_list() {
    blocked_list = ll_create_base_linked_list();
    if (blocked_list == NULL) return -1;
    return 1;
}

int add_blocked_nick(char * nick) {
    if (is_nick_blocked(nick) == 1) return 0;
    
    blocked_nick_t * new_blocked_nick = create_blocked_nick(nick);
    if (check_null_error_stderr(new_blocked_nick, "malloc in for new blocked nick failed")) {
        return -1;
    }
    if (ll_add_node(blocked_list, new_blocked_nick) == -1) {
        free(new_blocked_nick);
        return -1;
    }
    return 1;
}

int remove_blocked_nick(char * nick) {
    return ll_remove_on_match(blocked_list, match_blocked_nick, nick, free_blocked_nick);
}

void free_blocked_nick_list() {
    ll_free_linked_list(blocked_list, free_blocked_nick);
}

void print_blocked_nick_list() {
    ll_do_on_each(blocked_list, print_blocked_nick);
}

int is_nick_blocked(char * nick) {
    return ll_contains_matching_data(blocked_list, match_blocked_nick, nick);
}

int blocked_nick_list_size() {
    return ll_base_list_size(blocked_list);
}

blocked_nick_t * get_blocked_nick_by_nick(char * nick) {
    blocked_nick_t * tmp = ll_pop_on_match(blocked_list, match_blocked_nick, nick);
    return tmp;
}
