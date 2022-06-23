#include "upush_client.h"

void signal_handler() {
    quit_client();
}

void validate_input_params(int argc, char * argv[]) {
    
    if (argc != 6) { 
        printf(" ./upush_client <nick> <ip address> <port> <timeout> <loss probability>\n");
        exit(EXIT_SUCCESS); 
    }
    
    if (is_word_valid_nick(argv[1]) != 1) {
        printf("Invalid nickname: %s\n", argv[1]);
        exit(EXIT_SUCCESS);
    }
    
    struct sockaddr_in dummy;
    if (inet_pton(AF_INET, argv[2], &dummy.sin_addr.s_addr) == 0) {
        printf("Invalid ip address: %s\n", argv[2]);
        exit(EXIT_SUCCESS);
    }
    
    int port = atoi(argv[3]);
    if (port < 1024 || port > 65535) {
        printf("Invalid port: %s\n", argv[3]);
        exit(EXIT_SUCCESS);
    }

    int timeout = atoi(argv[4]);
    if (timeout < 0) {
        printf("Invalid timeout: %s", argv[4]);
        exit(EXIT_SUCCESS);
    }

    int loss_probability = atoi(argv[5]);
    if (loss_probability < 0 || loss_probability > 100 ) {
        printf("Invalid loss probability: %s", argv[5]);
        exit(EXIT_SUCCESS);
    }
}

void quit_client() {
    free_contact_list();
    free_client_server();
    free_blocked_nick_list();
    close(fd);
    free(client_nick);
    exit(EXIT_SUCCESS);
}

int is_valid_client_message(char * message) {

    int msg_len = strlen(message);

    if (msg_len >= 1400) {
        fprintf(stderr, "Message must be shorter than 1400 chars\n");
        return 0;
    }

    for (int i = 0; i < msg_len; i++) {
        if (!isascii(message[i])) {
            fprintf(stderr, "Message must contain only ASCII symbols\n");
            return 0;
        }
    }
    return 1;
}

int parse_client_input(char * buf, char * recipient_nick, char * recipient_message) {
    
    char * parsed_buf [BUFSIZE];

    // we must be able to parse message to at least two words separated by space
    if (parse_message(buf, parsed_buf, 2) < 2) {
        printf("Message should be formatted as \"@username hello!\"\n");
        return 0;
    }
    if(!is_word_valid_nick(parsed_buf[0])) {
        printf("Nickname was not a valid nickname\n");
        return 0;
    }
    if (!is_valid_client_message(parsed_buf[1])) return 0;

    strncpy(recipient_nick, parsed_buf[0], 20);
    strncpy(recipient_message, parsed_buf[1], 1399);
    recipient_message[1399] = 0;
    
    return 1;
}

int main (int argc, char * argv[]) {
    
    struct sockaddr_in my_addr; // for binding to our socket
    char buf [BUFSIZE];         // for parsing incoming messages
    int rc;                     // for validating reads 
    int ret;                    // for checking return values

    fd_set master_fds;                  // for keeping all of our fds (STDIN, and socket)
    fd_set copy_fds;                    // for using in select-functions, since fds are removed from the set used in select
    
    struct timeval timeout;             // short time used for SELECT func in main event loop
    struct timeval last_heartbeat_time; // time of last heartbeat
    struct timeval now;                 // to check current time after SELECT and see if we need to do a new heartbeat

    char * parsed_buf [BUFSIZE];

    char tmp_nick_buf [BUFSIZE];        // buffer for getting nick from server
    struct sockaddr_in tmp_addr_buf;    // buffer for getting addr from server

    signal(SIGINT, signal_handler);             // overwrite ctrl+c to run our quit function

    validate_input_params(argc, argv);         // check the input, and terminate if it's bad

    try_to_set_loss_probability(argv[5]);      // set the loss probability (in common.c)

    create_IPv4_UDP_socket(&fd);                // set up and bind socket (in common.c)
    if (check_error_perror(fd, "socket")) return EXIT_FAILURE;
    
    ret = set_values_in_IPv4_addr(&my_addr, 0, 0);
    if (check_error_perror(ret, "inet_pton")) return EXIT_FAILURE;    // setting 0 as port in bind will let OS choose an available port (in common.c)
    if (check_zero_error_stderr(ret, "inet_pton: IP invalid")) return EXIT_FAILURE;
    
    bind_sockaddr_to_fd(fd, &my_addr);
    if (check_error_perror(fd, "bind")) return EXIT_FAILURE;

    // global variables for convenience
    timeout_sec = atoi(argv[4]);    // the timeout value from params
    if (init_client_server() == -1) exit(EXIT_FAILURE);           // initiate the client_server
    if (init_contact_list() == -1) exit(EXIT_FAILURE);            // initiate the contact list in contact_list.h
    if (init_blocked_nick_list() == -1) exit(EXIT_FAILURE);       // initiate the blocked_nick_list in blocked_nick_list.h
    client_nick = strdup(argv[1]);                                // this clients nick
    if (check_null_error_stderr(client_nick, "malloc of client nick failed")) quit_client();

    ret = set_values_in_IPv4_addr(server->addr, argv[2], atoi(argv[3])); // setup server address (in common.c)
    if (check_error_perror(ret, "inet_pton")) quit_client();
    if (check_zero_error_stderr(ret, "inet_pton: IP invalid")) quit_client();


    // prepare for first select call
    FD_ZERO(&master_fds);
    FD_SET(fd, &master_fds); // add the network socket

    send_reg_msg_to_server(); // send a registration message to the server (in client_to_server_com.c)
    // error checking done in function

    set_time(&timeout, timeout_sec, 0); // reset the timer with global timeout value
    ret = select(
        FD_SETSIZE,
        &master_fds,
        0,
        0,
        &timeout
    );
    if (check_error_perror(ret, "first select")) quit_client();
    if (check_zero_error_stderr(ret, "Server did not respond. Terminating\n")) quit_client();

    ret = gettimeofday(&last_heartbeat_time, NULL);
    if (check_error_perror(ret, "gettimeofday")) quit_client();

    printf("\nConnected\n\n");
    
    // prepare for main event loop
    FD_ZERO(&master_fds);
    FD_SET(fd, &master_fds); // add the network socket
    FD_SET(STDIN_FILENO, &master_fds); // add stdin
    
    while (1) {

        copy_fds = master_fds; // select is destructive, so it's convenient to keep a master set

        set_time(&timeout, 0, 500000); // let select timeout every half second

        int socketCount = select(
            FD_SETSIZE,
            &copy_fds,
            0,
            0,
            &timeout
        );
        if (check_error_perror(socketCount, "select")) quit_client();

        ret = gettimeofday(&now, NULL);
        if (check_error_perror(ret, "gettimeofday")) quit_client();

        // check if we need to do new heartbeat
        if (now.tv_sec > last_heartbeat_time.tv_sec + 10) {
            ret = gettimeofday(&last_heartbeat_time, NULL);
            if (check_error_perror(ret, "gettimeofday")) quit_client();
            send_reg_msg_to_server();   // send a new registration message to server
            if (check_error_perror(ret, "sendto")) quit_client();
            continue;
        }

        // attempt to resend messages if any timers are upp
        resend_pending_messages();

        // did we receive input from client stdin?
        if (FD_ISSET(STDIN_FILENO, &copy_fds)) {  

            get_string(buf, BUFSIZE); // reads from stdin into buf (copied from CBRA)
            
            if (strcmp(buf, "QUIT") == 0) quit_client();

            if (string_starts_with(buf, "UNBLOCK")) {
                // parse the input into char ptr array. If it gets more than 2 elements, there are too many spaces
                if (parse_message(buf, parsed_buf, 3) == 2) {
                    if (remove_blocked_nick(parsed_buf[1])) printf("Nick \"%s\" was removed from blocked list\n", parsed_buf[1]);
                    else printf("Nick \"%s\" was not on the blocked list\n", parsed_buf[1]);
                } else printf("Invalid UNBLOCK input, try \"UNBLOCK username\"\n");
                continue;
            }

            if (string_starts_with(buf, "BLOCK")) {
                // parse the input into char ptr array. If it gets more than 2 elements, there are too many spaces
                if (parse_message(buf, parsed_buf, 3) == 2) {
                    ret = add_blocked_nick(parsed_buf[1]);
                    if (ret == -1) quit_client();
                    if (ret == 1) printf("Nick \"%s\" was added to blocked list\n", parsed_buf[1]);
                    if (ret == 0) printf("Nick \"%s\" was already on the blocked list\n", parsed_buf[1]);
                } else printf("Invalid BLOCK input, try \"BLOCK username\"\n");
                continue;
            }

            if (string_starts_with(buf, "@")) {
                char recipient_nick [20];
                char recipient_message[1400];
                // parse and validate the input and put it into recipient_nick and recipient_message
                if (parse_client_input(&buf[1], recipient_nick, recipient_message)) {
                    if (!is_nick_blocked(recipient_nick)) { // are we allowed to send to this nick?
                        contact_t * recipient;
                        recipient = find_contact_by_nick(recipient_nick); // try to find the contact in cache
                        if (recipient == NULL) { // if we did not have it in cache, look it up on server
                            ret = lookup_user_on_server(recipient_nick, tmp_nick_buf, &tmp_addr_buf);
                            if ( ret == -1) quit_client();
                            if ( ret == 1) { // if lookup was successful, tmp_nick_buf and addr are set
                                recipient = create_contact(tmp_nick_buf, &tmp_addr_buf); // create the new contact
                                if (recipient == NULL) quit_client();
                                if (add_contact_to_contact_list(recipient) == -1) quit_client(); // add it to our contact list
                            } else {
                                fprintf(stderr, "NICK %s NOT REGISTERED\n\n", recipient_nick);
                                continue;
                            }
                        } 
                        message_t * new_message = create_message(recipient_message); // create the message
                        if (new_message == NULL) quit_client();
                        
                        add_message_to_contact_message_list(new_message, recipient); // adds message to back of message queue
                         // if this the first message on the queue, we should start sending
                        if (peek_message(recipient->message_list) == new_message) send_next_message_to_contact(recipient);          
                    } else printf("The nick %s is on your blocked list. Type \"UNBLOCK %s\" to remove it.\n\n", recipient_nick, recipient_nick);
                } // error is printed in client_parse_input
            } else printf("Error: You must specify who you @, try \"@username hello!\"\n\n");
            continue;
        }

        // is there new message on our socket?
        if (FD_ISSET(fd, &copy_fds)) { 

            struct sockaddr_in src_addr;
            socklen_t addrlen = sizeof(struct sockaddr_in); // must be initialized to keep address from recvfrom

            rc = recvfrom(
                fd, 
                buf, 
                BUFSIZE, 
                0,
                (struct sockaddr *) &src_addr,
                &addrlen
            );
            if (check_error_perror(rc, "recvfrom")) quit_client();
            buf[rc] = 0;

            // could for exampel look like “PKT nummer FROM fra_nick TO to_nick MSG tekst”
            char * parsed [BUFSIZE];

            // is message from the server?
            if (is_same_address(&src_addr, server->addr)) {
                // replies for heartbeats end up here, this code is not really necessary
                
                // there is a message from the server
                ret = parse_message(buf, parsed, 3);
                if (strcmp(parsed[0], "ACK") == 0) {
                    if (strcmp(parsed[1], "0") == 0) server->last_ack_received = 0;
                    if (strcmp(parsed[1], "1") == 0) server->last_ack_received = 1;
                }
                continue;
            } 

            // We now know that message must come from another client

            // max 8 words in parsed message, meaning "tekst" with spaces may be in element i=7
            // returns amount of elements in array            
            ret = parse_message(buf, parsed, 8);
            if (ret == 0) {
                printf("Parsing of message failed\n");
                continue;
            }

            // if index = 0 is "ACK", we should find message in queue and mark as ACKed
            if (strcmp(parsed[0], "ACK") == 0) {
                // printf("We received an ACK from a client\n");
                contact_t * acking_contact = find_contact_by_addr(&src_addr);
                // we should never not be able to find the acking contact
                if (acking_contact != 0) register_ack_message(atoi(parsed[1]), acking_contact);
                continue;
            }

            // we have to make sure the message was for us
            if (strcmp(parsed[5], client_nick) != 0) {
                printf("\nReceived a message which was not intended for this client. Skipping.\n");
                continue;
            }

            // we received a new message from a contact which was intended for us
            char * sender_nick = parsed[3];
            contact_t * sender = find_contact_by_nick(sender_nick);
            if (sender == NULL) {
                if (lookup_user_on_server(sender_nick, tmp_nick_buf, &tmp_addr_buf)) {
                    sender = create_contact(tmp_nick_buf, &tmp_addr_buf); // create the new contact
                    if (sender == NULL) quit_client();
                    add_contact_to_contact_list(sender); // add it to our contact list
                } else {
                    fprintf(stderr, "NICK %s NOT REGISTERED\n\n", sender_nick);
                    // we don't trust it, since we cannot find user on server.
                    // don't send ACK at all
                    continue;
                }
            }
            
            int pkt_number = atoi(parsed[1]);

            // is this a new message which we haven't seen before?
            if ( pkt_number != sender->last_pkt_received) {
                // this is now the latest known package number 
                sender->last_pkt_received = pkt_number; 
                // only print messages if sender's nick is not on blocked list
                if (!is_nick_blocked(sender->nick)) printf("\n[%s]: %s\n\n", sender->nick, parsed[7]);
            }

            char response [BUFSIZE];
            sprintf(response, "ACK %d OK", pkt_number);

            int wc = send_packet(
                fd,
                response,
                strlen(response),
                0,
                (struct sockaddr *) sender->addr,
                sizeof(struct sockaddr)
            );
            if (check_error_perror(wc, "send_packet ack")) quit_client();
            // printf("Sent response: %s\n", response); 
        }
    }
    quit_client();
}