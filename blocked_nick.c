#include "blocked_nick.h"

blocked_nick_t * create_blocked_nick(char * nick) {
    blocked_nick_t * tmp = malloc(sizeof(blocked_nick_t));
    if (check_null_error_stderr(tmp, "Malloc of blocked nick failed")) return NULL;
    
    tmp->nick = strdup(nick);
    if (check_null_error_stderr(tmp->nick, "Malloc of blocked nick failed")) {
        free(tmp);
        return NULL;
    }
    return tmp;
}

void free_blocked_nick(void * data) {
    blocked_nick_t * tmp = (blocked_nick_t *) data;
    free(tmp->nick);
    free(tmp);
}

int match_blocked_nick(void * data, void * other_data) {
    blocked_nick_t * tmp = (blocked_nick_t *) data;
    if (strcmp(tmp->nick, other_data) == 0) return 1;
    return 0;
}

void print_blocked_nick(void * data) {
    blocked_nick_t * tmp = (blocked_nick_t *) data;
    printf("Blocked nick: %s", tmp->nick);
}