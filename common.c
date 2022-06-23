#include "common.h"

int check_error_perror(int res, char * msg) {
    if (res == -1) {
        perror(msg);
        return 1;
    }
    return 0;
}

int check_null_error_perror(void * ptr, char *msg) {
    if ( ptr == NULL) {
        perror(msg);
        return 1;
    }
    return 0;
}

int check_zero_error_perror(int res, char *msg) {
    if ( res == 0) {
        perror(msg);
        return 1;
    }
    return 0;
}

int check_null_error_stderr(void * ptr, char * msg) {
    if (ptr == NULL) {
        fprintf(stderr, "%s\n", msg);
        return 1;
    }
    return 0;
}

int check_zero_error_stderr(int res, char *msg) {
    if ( res == 0) {
        fprintf(stderr, "%s\n", msg);
        return 1;
    }
    return 0;
}

void create_IPv4_UDP_socket(int * fd) {
    *fd = socket(AF_INET, SOCK_DGRAM, 0);
}

int set_values_in_IPv4_addr(struct sockaddr_in * addr, char * ip, int port) {

    addr->sin_family = AF_INET; // is of family IPv4
    addr->sin_port = htons(port); // use htons() to format port from int
    if (!ip) {
        addr->sin_addr.s_addr = INADDR_ANY;
        return 1;
    }
    
    return inet_pton(AF_INET, ip, &addr->sin_addr.s_addr); // use inet_pton to format ip
}

int bind_sockaddr_to_fd(int fd, struct sockaddr_in * my_addr) {
    return bind(fd, (struct sockaddr *) my_addr, sizeof(struct sockaddr));
}


// NOTE: This function is copied from a CBRA video
void get_string(char * buf, int size) {
    char c; 
    
    fgets(buf, size, stdin);
    if (buf[strlen(buf) -1] == '\n') {
        buf[strlen(buf) -1] = 0;
    } else {
        while( (c = getchar() ) != '\n' && c != EOF);
    }
}

// returns 1 if big_string starts with sub_string, else 0
int string_starts_with(char * big_string, char * sub_string) {
    int i = 0;
    int max_len = strlen(sub_string);

    while (i < max_len) {
        if (big_string[i] != sub_string[i]) return 0;
        i++;
    }
    return 1;
}

void try_to_set_loss_probability(char * p_loss) {
    
    int loss_int;
    float decimal;

    loss_int = atoi(p_loss);
    if (loss_int < 0 || loss_int > 100) {
        fprintf(stderr, "Loss probability input must be > 0 and < 100. Try again\n");
        exit(EXIT_FAILURE);
    }
    decimal = (float) loss_int / 100;
    
    set_loss_probability(decimal); 

    // also add a seed to the random number generator.
    // otherwise, the rand48() call will not be very random at all
    struct timeval now;
    gettimeofday(&now, NULL);
    srand48(now.tv_sec);
    
    printf("Loss probability was set to: %.0f %%\n", decimal * 100);
}

int parse_message(char * msg, char * parsed [], int max_elems) {
    
    if (max_elems <= 1) return 0; // must parse into at least 2 elems

    parsed[0] = strtok(msg, " "); // insert the msg into strtok, and get first word
    
    if (max_elems == 0 || parsed[0] == NULL) return 0; // if there was no first word, or input max was 0, return

    int i = 1;
    while (i < max_elems-1) {
        parsed[i] = strtok(NULL, " "); // insert next word into next pos of array
        if (parsed[i] == NULL) {
            return i; // if the word we just added is NULL, then break without increasing i
        }
        i++;
    }

    parsed[max_elems-1] = strtok(NULL, "");     // fit remainder of string into last allowed element
    if (parsed[max_elems-1] == NULL) return i;  // if the last thing we added was NULL, just return i without increasing
    
    i++;        // increase i since we successfully added last element
    return i;
}

// returns 1 if word is valid nick
int is_word_valid_nick(char * word) {
    
    int word_len = strlen(word);
    if (word == NULL || word_len >= 20 ) return 0; // return 0 if word is NULL


    for (int i = 0; i < (int)strlen(word); i++) {
        if (i >= 20) return 0;
        if (!isascii(word[i])) return 0; // return false if not all ascii 
        if (isblank(word[i])) return 0; // return false if there are spaces or tabs
        if (word[i] == '\n') return 0; // return false if has newline
    }
    return 1;
}

// returns 1 if word is an integer
int is_word_number (char * word) {
    if (word == NULL) return 0; // return 0 if word is NULL
    for (int i = 0; i < (int)strlen(word); i++) {
        if (isdigit(word[i]) == 0) return 0; // if not all chars in number are digits, return
    }
    return 1;
}

// returns 1 if addresses are identical on all values, else 0
int is_same_address(struct sockaddr_in * first_addr, struct sockaddr_in * second_addr) {
    if (first_addr == NULL || second_addr == NULL) return 0; // neither can be NULL
    if (first_addr->sin_family != second_addr->sin_family) return 0; // must have same family
    if (first_addr->sin_port != second_addr->sin_port) return 0; // must have same port
    if (first_addr->sin_addr.s_addr != second_addr->sin_addr.s_addr) return 0; // must have same ip
    return 1;
}

void toggle_binary(int * number) {
    printf("Toggle recieved: %d\n", *number);
    if (*number == 0) *number = 1;
    if (*number == 1) *number = 0;
    printf("Toggle set: %d\n", *number);
}

int get_ip_and_port_from_sockaddr_in(char * ip_address, int * client_port, struct sockaddr_in * src_addr) {

    const char * ptr;
    ptr = inet_ntop(
        AF_INET,                    // int AF  
        &src_addr->sin_addr.s_addr, // const void *src
        ip_address,             // char *dst
        sizeof(struct sockaddr)     // socklen_t size
    );
    if (ptr == NULL) {
        perror("inet_ntop");
        return 0;
    }

    *client_port = ntohs(src_addr->sin_port);
    if (*client_port < 1024 || *client_port > 65535 ) {
        fprintf(stderr, "Invalid port %d\n", *client_port);
        return 0;
    }

    return 1;
}