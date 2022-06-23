#include "client_to_client_com.h"

void register_ack_message(int pkt_number, contact_t * contact) {
    contact->last_ack_received = pkt_number;
    // printf("Registered ack %d for %s\n", contact->last_ack_received, contact->nick);
    message_t * msg = pop_message(contact->message_list); // we know this has been sent to the target client
    free_message(msg);

    send_next_message_to_contact(contact);
}

void send_message_to_contact(contact_t * contact, message_t * message) {
    
    int wc;
    char buf [BUFSIZE];
    
    sprintf(buf, "PKT %d FROM %s TO %s MSG %s", contact->last_pkt_sent, client_nick, contact->nick, message->payload);
    message->attempts++;
    
    wc = gettimeofday(contact->time_of_last_send, NULL);
    if (check_error_perror(wc, "gettimeofday")) quit_client();

    wc = send_packet(
        fd,
        buf,
        strlen(buf),
        0,
        (struct sockaddr *) contact->addr,
        sizeof(struct sockaddr)
    );
    if (check_error_perror(wc, "sendto, contact")) quit_client();
    // printf("Sent %s\n", buf);
}

void resend_message_to_contact(contact_t * contact) {

    // printf("Resending a packet for %s", contact->nick);

    message_t * message = peek_message(contact->message_list);
    if (message->attempts == 4) {
        // we have already tried 4 times to send this message
        fprintf(stderr, "NICK %s UNREACHABLE, contact and message queue deleted. Sorry.\n", contact->nick);
        dettach_contact_from_contact_list(contact);
        free_contact(contact);
        return;
    }

    if (message->attempts == 2) {
        // we should do a new lookup before we proceed
        char tmp_nick_buf [BUFSIZE];
        struct sockaddr_in tmp_addr_buf;

        if (lookup_user_on_server(contact->nick, tmp_nick_buf, &tmp_addr_buf) == 1) {
            // memcopy the new address to the existing contact, since memory is already allocated
            printf("must lookup user\n");
            memcpy(contact->addr, &tmp_addr_buf, sizeof(struct sockaddr_in));
        } else {
            // stop trying to deliver the message
            message = pop_message(contact->message_list);
            printf("Could not send message:\n \"%s\"\n You can try sending it again.\n", message->payload);
            free_message(message);
            return;
        } 
    }
    send_message_to_contact(contact, message);
}

void send_next_message_to_contact(contact_t * contact) {
    message_t * message = peek_message(contact->message_list);
    if (message != NULL) {
        toggle_last_pkt_sent(contact);
        // printf("Last packet sent er nå: %d\n", contact->last_pkt_sent);
        send_message_to_contact(contact, message);
    }
}

void resend_pending_messages() {

    int ret;

    if (contact_list_size() == 0) return;

    contact_iterator_t * contact_iterator = get_contact_iterator();
    if (contact_iterator == NULL) quit_client();

    while(contact_iterator_has_next(contact_iterator)) {
        
        contact_t * contact = contact_iterator_next(contact_iterator);

        if (message_list_size(contact->message_list) > 0) {
            
            // printf("There messages in queue of %s which are not fully sent\n", contact->nick);

            struct timeval now;
            ret = gettimeofday(&now, NULL); // find current time
            if (check_error_perror(ret, "gettimeofday")) quit_client();

            // if it's time to resend last message
            if (now.tv_sec > contact->time_of_last_send->tv_sec + timeout_sec) 
                resend_message_to_contact(contact);
        }
    }
    free_contact_iterator(contact_iterator);
}