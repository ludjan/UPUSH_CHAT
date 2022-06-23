#include "contact_list.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    contact_t * contact;
    struct sockaddr_in addr;

    init_contact_list();

    set_values_in_IPv4_addr(&addr, "123.123.123.123", 123124);
    contact = create_contact("Berit", &addr);
    add_contact_to_contact_list(contact);

    set_values_in_IPv4_addr(&addr, "111.111.111.111", 14141);
    contact = create_contact("Johan", &addr);
    add_contact_to_contact_list(contact);
    
    set_values_in_IPv4_addr(&addr, "222.222.222.222", 14551);
    contact = create_contact("Erik", &addr);
    add_contact_to_contact_list(contact);
    
    set_values_in_IPv4_addr(&addr, "161.6.116.6", 14221);
    contact = create_contact("Adam", &addr);
    add_contact_to_contact_list(contact);

    contact_iterator_t * contact_iterator = get_contact_iterator();

    while (contact_iterator_has_next(contact_iterator)) {
        contact_t * tmp = contact_iterator_next(contact_iterator);
        print_contact(tmp);
    }
}