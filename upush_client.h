#ifndef MY_UPUSH_CLIENT
#define MY_UPUSH_CLIENT

#include "common.h"
#include "send_packet.h"
#include "signal.h"

#include "client_server.h"
#include "contact.h"
#include "contact_list.h"
#include "client_to_server_com.h"
#include "client_to_client_com.h"
#include "blocked_nick_list.h"
#include "time_functions.h"

int fd;
char * client_nick;
int timeout_sec;

void reset_timeout(struct timeval * timeout);

int is_valid_client_message(char * message);

int parse_client_input(char * buf, char * recipient_nick, char * recipient_message);

void quit_client();

void client_check_error(int res, char * msg);

#endif