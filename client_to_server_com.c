#include "client_to_server_com.h"

void send_reg_msg_to_server() {

    int wc;
    char buf [BUFSIZE];

    sprintf(buf, "PKT %d REG %s", next_message_number_server(server), client_nick);

    wc = send_packet(
        fd,
        buf,
        strlen(buf),
        0,
        (struct sockaddr *) server->addr,
        sizeof(struct sockaddr)
    );
    
    if (check_error_perror(wc, "sendto")) quit_client();
}

int send_blocking_lookup_message(char * message, char * tmp_nick_buf, struct sockaddr_in * tmp_addr_buf) {
    
    int wc, rc;
    char inc_buf [BUFSIZE]; //   for the potential replies we get. We don't want to overwrite the message we send in case we receive some other message
    
    struct sockaddr_in src_addr; // for keeping address of potential response
    socklen_t addrlen = sizeof(struct sockaddr_in); // must initiate socklen to use it in recvfrom()

    // prepare for lookup loop
    fd_set lookup_set;
    FD_ZERO(&lookup_set);
    FD_SET(fd, &lookup_set);

    // send the lookup message
    wc = send_packet(
        fd,
        message,
        strlen(message),
        0,
        (struct sockaddr *) server->addr,
        sizeof(struct sockaddr)
    );
    if (check_error_perror(wc, "send, lookup")) return -1;

    struct timeval timeout;
    set_time(&timeout, timeout_sec, 0);

    // listen until timeout breaks 
    while (1) {

        fd_set copy = lookup_set;

        int socketCount = select(
            FD_SETSIZE,
            &copy,
            0,
            0,
            &timeout   // timeout
        );
        if (check_error_perror(socketCount, "select")) quit_client();
        if (socketCount == 0) return -1; // check if select timed out 

        // if it was not a timeout, we must have a message on fd
        rc = recvfrom(
            fd, 
            inc_buf, 
            BUFSIZE, 
            0,
            (struct sockaddr *) &src_addr,
            &addrlen
        );
        if (check_error_perror(rc, "recvfrom")) return -1;

        inc_buf[rc] = 0;

        // printf("%s\n", inc_buf);
        
        // is message from server?
        if (is_same_address(&src_addr, server->addr)) {
            
            if (strstr(inc_buf, "OK")) continue; // if this is an ACK for a heartbeat and we receive "ACK 0/1 OK", ignore it
            
            break; // this must be a reply for our lookup message
        }
        // if message was not from server, we ignore it and keep listening
    }    

    if (strstr(inc_buf, "0")) server->last_ack_received = 0;
    if (strstr(inc_buf, "1")) server->last_ack_received = 1;

    if (strstr(inc_buf, "NOT FOUND")) return 0;

    char * parsed_buf [BUFSIZE];
    char * contact_nick;
    char * contact_ip;
    int contact_port;
    struct sockaddr_in contact_addr;
    
    // message is on form: “ACK nummer NICK nick IP adresse PORT port”
    parse_message(inc_buf, parsed_buf, 8);

    contact_nick = parsed_buf[3];
    contact_ip = parsed_buf[5];
    contact_port = atoi(parsed_buf[7]);

    rc = set_values_in_IPv4_addr(&contact_addr, contact_ip, contact_port);
    if (check_error_perror(rc, "inet_pton")) return -1;                    // may return -1 
    if (check_zero_error_stderr(rc, "inet_pton: IP invalid")) return -1;   // may return 0 on invalid IP


    strncpy(tmp_nick_buf, contact_nick, 21);
    memcpy(tmp_addr_buf, &contact_addr, sizeof(struct sockaddr_in));

    // buf has been set, so we return 1

    return 1;
}

int lookup_user_on_server(char * target_nick, char * tmp_nick_buf, struct sockaddr_in * tmp_addr_buf) {

    int ret;
    char out_buf [BUFSIZE]; // for the message we are about to send
    
    // construct the message
    sprintf(out_buf, "PKT %d LOOKUP %s", next_message_number_server(server), target_nick);
    // printf("%s\n", out_buf);

    printf("\nSending blocking lookup message, ignoring input\n");

    // try sending a blocking lookup message 3 times
    for (int i = 0; i < 3; i++) {
        ret = send_blocking_lookup_message(out_buf, tmp_nick_buf, tmp_addr_buf);
        if (ret == 0) return 0; // user does not exist
        if (ret == 1) {
            printf("Lookup finished successfully!\n\n");
            return 1; // tmp_nick_buf and tmp_addr_buf is set
        }
        // if -1 was returned, we had a timeout and may try again
    }

    // we attempted lookup 3 times. Seems like server is not responding.
    fprintf(stderr, "Server is not responding, terminating.\n");

    quit_client();

    // this will never happen, but compiler complains
    return -1;
}