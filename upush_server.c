#include "upush_server.h"

volatile sig_atomic_t keep_going = 1;

void signal_handler() {
    keep_going = 0;
}

void quit_server() {
    close(fd);
    free_client_list();
    exit(EXIT_SUCCESS);
}

// tries to create and bind an address for receiving UDP messages
void try_to_setup_and_bind_socket(int * fd, char * port_c) {
    
    int ret;

    struct sockaddr_in server_addr;
    int server_port;

    // convert input char * to int
    server_port = atoi(port_c);

    // create and IPv4 UDP socket
    create_IPv4_UDP_socket(fd);
    if(check_error_perror(*fd, "socket")) exit(EXIT_FAILURE);

    // create servers address
    ret = set_values_in_IPv4_addr(&server_addr, 0, server_port);
    if (check_error_perror(ret, "inet_pton")) exit(EXIT_FAILURE);                    // may return -1 
    if (check_zero_error_stderr(ret, "inet_pton: IP invalid")) exit(EXIT_FAILURE);   // may return 0 on invalid IP

    // bind the socket and the servers address
    ret = bind_sockaddr_to_fd(*fd, &server_addr);
    if (check_error_perror(ret, "bind")) quit_server();

    printf("Socket created and bound successfully on port %d\n", server_port);
}

// registers the nick, ip and port of client
void register_client(char * parsed [], struct sockaddr_in * src_addr, int fd, socklen_t addrlen) {
      
    char * nick = parsed[3];
    char * number = parsed[1];
    int ret;

    char ip_address [BUFSIZE];
    int client_port;

    // get address of reg message
    ret = get_ip_and_port_from_sockaddr_in(ip_address, &client_port, src_addr);
    if (check_zero_error_stderr(ret, "Could not get ip and port from sockaddr_in")) quit_server();

    // printf("ip_address = %s\n", ip_address);

    server_client_t * tmp;
    if ( (tmp = find_client_by_nick(nick) ) != 0 )  {
        if (strcmp(tmp->ip_address, ip_address) == 0) {
            if (update_client_ip_address(tmp, ip_address) == -1) quit_server();
        }
        if (tmp->port != client_port) {
            update_client_port(tmp, client_port);
        }
        if (update_client_last_heartbeat(tmp) == -1) quit_server();
    } else {
        tmp = create_client(nick, ip_address, client_port);
        if (tmp == NULL) quit_server();
        if (add_server_client_to_list(tmp) == -1) quit_server();
    }
    
    char response [BUFSIZE];
    sprintf(response, "ACK %s OK", number); // construct the response

    ret = send_packet( 
        fd,                             // int sock 
        response,                       // void* buffer
        strlen(response),               // size_t size
        0,                              // int flags
        (struct sockaddr *) src_addr,   // struct sockaddr* addr, 
        addrlen                         // socklen_t addrlen 
    );
    if (check_error_perror(ret, "select")) quit_server();
    printf("Sent response: %s\n", response);
}

// looks up a nick in memory, and replies
void lookup_client(char * parsed [], struct sockaddr_in * src_addr, int fd, socklen_t addrlen) {
 
    int wc;
    char * number = parsed[1];
    char * target_nick = parsed[3];
    char response [BUFSIZE];
    

    server_client_t * tmp;
    if ( ( tmp = find_client_by_nick(target_nick) ) == NULL) {
        sprintf(response, "ACK %s NOT FOUND", number);
    } else {
        if ( get_seconds_since_last_heartbeat(tmp) > 30 ) {
            printf("Removing client %s\n", tmp->nick);
            sprintf(response, "ACK %s NOT FOUND", number);
            dettach_client_from_list(tmp);
            free_client(tmp);
        } else {
            sprintf(response, "ACK %s NICK %s IP %s PORT %d", number, tmp->nick, tmp->ip_address, tmp->port);        
        }
    }

    wc = send_packet( 
        fd,                                     // int sock 
        response,                               // void* buffer
        strlen(response),                       // size_t size
        0,                                      // int flags
        (struct sockaddr *) src_addr,           // struct sockaddr* addr, 
        addrlen                                 // socklen_t addrlen 
    );
    if (check_error_perror(wc, "send_packet")) quit_server();
    printf("Sent response: %s\n", response);
}

// returns 1 if array is a valid lookup message
int is_lookup_message(char * parsed_buf [], int no_args) {
    if (no_args != 4) return 0;                                                     // must have 4 args
    if (parsed_buf[0] != NULL && strcmp(parsed_buf[0], "PKT") != 0) return 0;       // validate PKT
    if (!is_word_number(parsed_buf[1])) return 0;                                   // validate number
    if (parsed_buf[2] != NULL && strcmp(parsed_buf[2], "LOOKUP") != 0) return 0;    // validate CMD
    if (!is_word_valid_nick(parsed_buf[3])) return 0;                               // validate NICK
    return 1;                                                                       // on success, return 1
}
// returns 1 if array is a valid registration message
int is_reg_message(char * parsed_buf [], int no_args) {
    if (no_args != 4) return 0;                                                 // must have 4 args
    if (parsed_buf[0] != NULL && strcmp(parsed_buf[0], "PKT") != 0) return 0;   // validate PKT
    if (!is_word_number(parsed_buf[1])) return 0;                               // validate number
    if (parsed_buf[2] != NULL && strcmp(parsed_buf[2], "REG") != 0) return 0;   // validate CMD
    if (!is_word_valid_nick(parsed_buf[3])) return 0;                           // validate NICK
    return 1;                                                                   // on success, return 1 
}

void validate_input_params(int argc, char * argv[]) {
    
    if (argc != 3) { 
        printf(" ./upush_server <port> <loss probability>\n");
        exit(EXIT_SUCCESS); 
    }
    
    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        printf("Invalid port: %s\n", argv[3]);
        exit(EXIT_SUCCESS);
    }

    int loss_probability = atoi(argv[2]);
    if (loss_probability < 0 || loss_probability > 100 ) {
        printf("Invalid loss probability: %s", argv[5]);
        exit(EXIT_SUCCESS);
    }
}

int main (int argc, char * argv[]) {

    struct sockaddr_in src_addr;    // for keeping addresses of incoming messages
    socklen_t src_addr_len;         // for keeping length of incoming messages
    char inc_buf [BUFSIZE];         // for keeping the incoming messages
    
    fd_set master_fds;      // for keeping the original fd
    fd_set copy_fds;        // select call is destructive, so we copy the master each call
    
    int ret;
    int rc;

    validate_input_params(argc, argv);   // check the input, and terminate if it's bad

    if (argc != 3) {
        printf("./upush_server <port> <tapsannsynlighet>\n");
        return EXIT_SUCCESS;
    }

    try_to_set_loss_probability(argv[2]);

    // global variable defined in "upush_server.h" for keeping file descriptor
    try_to_setup_and_bind_socket(&fd, argv[1]);
    
    // linked list for keeping clients (global variable in server_client_list.h)
    if (init_server_client_list() == -1) return EXIT_FAILURE;

    signal(SIGINT, signal_handler); // overwrite ctrl+c

    src_addr_len = sizeof(struct sockaddr_in); // (addr_len must be initialized to store the address in recvfrom)

    FD_ZERO(&master_fds);
    FD_SET(fd, &master_fds);

    while(1) {
        
        printf("\n");

        copy_fds = master_fds;

        // wait until there is a message on the socket
        ret = select(
            FD_SETSIZE,
            &copy_fds,
            0,
            0,
            0 // no timeout
        );
        if (check_error_perror(ret, "select")) quit_server();

        rc = recvfrom(
            fd, 
            inc_buf, 
            BUFSIZE, 
            0,
            (struct sockaddr *) &src_addr,
            &src_addr_len
        );
        if (check_error_perror(rc, "recvfrom")) quit_server();

        if (*inc_buf == '\n') {
            printf("skipped ln\n");
            continue; // we ignore potential "empty" messages
        }
        inc_buf[rc] = 0;

        printf("Incoming message: %s\n", inc_buf);

        char * parsed_buf [BUFSIZE];
        rc = parse_message(inc_buf, parsed_buf, 4); // returns amount of arguments

        if (is_reg_message(parsed_buf, rc)) {
            register_client(parsed_buf, &src_addr, fd, src_addr_len);
            continue;
        };

        if (is_lookup_message(parsed_buf, rc)) {
            lookup_client(parsed_buf, &src_addr, fd, src_addr_len);
            continue;
        }
        printf("Skipped\n");
    }
}