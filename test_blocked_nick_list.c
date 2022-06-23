#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "upush_client.h"
#include "blocked_nick_list.h"
#include "common.h"

int main() {

    init_blocked_nick_list();

    printf("Is size 0?                  Expected 0, Actual %d\n", blocked_nick_list_size());
    printf("Is unknown nick blocked?    Expected 0, Actual %d\n", is_nick_blocked("Wilhelmutt"));

    printf("\n");

    // add a nick, check if it is blocked
    add_blocked_nick("Peter");
    printf("Added nick exists?          Expected 1, Actual %d\n", is_nick_blocked("Peter"));
    printf("Can we add same nick twice? Expected 0, Actual %d\n", add_blocked_nick("Peter"));
    printf("Is size 1?                  Expected 1, Actual %d\n", blocked_nick_list_size());

    printf("\n");

    // add another nick, check both are blocked
    add_blocked_nick("Ida");
    printf("Existing nick exists?       Expected 1, Actual %d\n", is_nick_blocked("Peter"));
    printf("Added nick exists?          Expected 1, Actual %d\n", is_nick_blocked("Ida"));
    printf("Is unknown nick blocked?    Expected 0, Actual %d\n", is_nick_blocked("Wilhelmutt"));
    printf("Is size 2?                  Expected 2, Actual %d", blocked_nick_list_size());

    
    printf("\n");

    // remove first nick, check it is no longer blocked, but second one is blocked
    remove_blocked_nick("Peter");
    printf("Removed nick exists?        Expected 0, Actual %d\n", is_nick_blocked("Peter"));
    printf("Existing nick exists?       Expected 1, Actual %d\n", is_nick_blocked("Ida"));
    

    printf("\n");

    remove_blocked_nick("Ida");
    printf("Removed nick exists?        Expected 0, Actual %d\n", is_nick_blocked("Ida"));
    printf("Is size 0?                  Expected 0, Actual %d\n", blocked_nick_list_size());

    // free list
    free_blocked_nick_list();

    printf("List was reinitialized\n");
    init_blocked_nick_list();

    add_blocked_nick("Peter");
    add_blocked_nick("Ida");

    printf("Is size 2?                  Expected 2, Actual %d\n", blocked_nick_list_size());

    free_blocked_nick_list();
    
    printf("Freed blocked_nick_list with existing nicks");


}