#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
#include "send_packet.h"

#define BUFSIZE 4096

// checks if input is -1,
// if so, perrors and returns 1
// else returns 0
int check_error_perror(int res, char* msg);

// checks if input is NULL
// if so, perrors and returns 1
// else returns 0 
int check_null_error_perror(void * ptr, char *msg);

// checks if input is 0
// if so, perrors and returns 1
// else returns 0
int check_zero_error_perror(int res, char *msg);

// checks if input is NULL
// if so, prints to stderr and returns 1
// else returns 0
int check_null_error_stderr(void * ptr, char * msg);

// checks if input is 0
// if so, prints to stderr and returns 1
// else returns 0
int check_zero_error_stderr(int res, char *msg);

// creates an IPv4 UDP socket and places file descriptor on fd
void create_IPv4_UDP_socket(int * fd);

// sets ip and port to addr and terminates on fail.
// if port = 0, use any port
// if ip is = NULL, use any IP
int set_values_in_IPv4_addr(struct sockaddr_in * addr, char * ip, int port);

// bind a sockaddr_in to an fd and terminate on fail
int bind_sockaddr_to_fd(int fd, struct sockaddr_in * my_addr);

// puts up to size characters from stdin into buf.
// sets terminating null byte in buf
void get_string(char * buf, int size);

// returns 1 if big_string starts with sub_string
// else returns 0
int string_starts_with(char * big_string, char * sub_string);

// sets loss probability if p_loss is a valid number between 0 and 100 (exclusive) as interpreted by assignment
void try_to_set_loss_probability(char * p_loss);


/**************
    parse_message

    Parse a msg into a supplied char pointer array, but only use max elements.
    if msg has more spaces than max, squeeze the remainder into last element 
    return amount of elements in parsed array
    
    Example:
    if msg = "How many elements do I get" and max = 3
    parsed will contain ["How", "many", "elements do I get", NULL, NULL, ... ]
**************/
int parse_message(char * msg, char * parsed [], int max);

// return 1 if param is a valid nickname, else 0
// nick name must:
// - not contain blanks or new lines
// - only contains ASCII symbols, else 0
// - contain 20 characters or less, (excluding null byte)
int is_word_valid_nick(char * msg);

// return 1 if param is a number (only contains digits), else 0
int is_word_number(char * msg);

// returns 1 if two sockaddr_in addresses are identical
int is_same_address(struct sockaddr_in * first_addr, struct sockaddr_in * second_addr);

// puts IP of src_addr and puts into ip_address as chars
// puts port of src_addr and puts into port as int
// returns 1 on succsess, else 0
int get_ip_and_port_from_sockaddr_in(char * ip_address, int * client_port, struct sockaddr_in * src_addr);

#endif