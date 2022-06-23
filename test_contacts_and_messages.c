#include "contact_list.h"
#include "message_list.h"
#include "common.h"

void test_add_and_pop() {

    printf("test_add_and_pop()\n");

    message_t * message;
    contact_t * contact;
    struct sockaddr_in addr;


    init_contact_list();

    set_values_in_IPv4_addr(&addr, "111.111.111.111", 14141);
    contact = create_contact("Johan", &addr);
    add_contact_to_contact_list(contact);
    
    message = create_message("Melding1");
    add_message_to_contact_message_list(message, contact);

    message = create_message("Melding2");
    add_message_to_contact_message_list(message, contact);

    message = create_message("Melding3");
    add_message_to_contact_message_list(message, contact);

    message = create_message("Melding4");
    add_message_to_contact_message_list(message, contact);

    message = create_message("Melding5");
    add_message_to_contact_message_list(message, contact);

    // print_contacts_and_messages();

    message = pop_message(contact->message_list);
    print_message(message);

    message = pop_message(contact->message_list);
    print_message(message);

    message = pop_message(contact->message_list);
    print_message(message);

    message = pop_message(contact->message_list);
    print_message(message);

    message = pop_message(contact->message_list);
    print_message(message);

    if ( peek_message(contact->message_list) != NULL ) {
        message = pop_message(contact->message_list);
        print_message(message);
    }

    // print_contacts_and_messages();

    free_contact_list(contact_list);
}

int main () {

    message_t * message;
    contact_t * contact;
    struct sockaddr_in addr;
    // int ret;
    
    char buffer [2000];
    int port;

    init_contact_list();
    
    set_values_in_IPv4_addr(&addr, "123.123.123.123", 123124);
    get_ip_and_port_from_sockaddr_in(buffer, &port, &addr);
    contact = create_contact("Berit", &addr);
    add_contact_to_contact_list(contact);

    set_values_in_IPv4_addr(&addr, "111.111.111.111", 14141);
    contact = create_contact("Johan", &addr);
    message = create_message("Melding");
    add_contact_to_contact_list(contact);
    add_message_to_contact_message_list(message, contact);

    set_values_in_IPv4_addr(&addr, "222.222.222.222", 14551);
    contact = create_contact("Erik", &addr);
    add_contact_to_contact_list(contact);
    message = create_message("Melding2");
    add_message_to_contact_message_list(message, contact);

    set_values_in_IPv4_addr(&addr, "161.6.116.6", 14221);
    contact = create_contact("Adam", &addr);
    add_contact_to_contact_list(contact);
    message = create_message("Melding3");
    add_message_to_contact_message_list(message, contact);

    // print_contacts_and_messages();

    contact = NULL;

    printf("Popper contact\n");    
    contact = pop_contact();
    print_contact(contact);
    free_contact(contact);

    print_contact_list(contact_list);

    printf("Printer Adam\n");
    contact = find_contact_by_nick("Adam");
    print_contact(contact);

    if (find_contact_by_nick("Willhelmutt") == 0) {
        printf("Det finnes ingen ved navn Willhelmutt\n");
    }

    printf("Oppdaterer Adam\n");
    set_values_in_IPv4_addr(&addr, "123.3.254.1", 11521);
    memcpy(contact->addr, &addr, sizeof(struct sockaddr_in));
    // print_contact_and_messages(contact);

    // printf("Fjerner %s fra listen\n", contact->nick);
    dettach_contact_from_contact_list(contact);
    free_contact(contact);
    
    message = create_message("Hei!");
    contact = find_contact_by_nick("Erik");
    add_message_to_contact_message_list(message, contact);

    // print_contact_and_messages(contact);

    free_contact_list();

    test_add_and_pop();

    return EXIT_SUCCESS;

}