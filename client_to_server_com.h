#ifndef CLIENT_TO_SERVER_COMMUNICATION
#define CLIENT_TO_SERVER_COMMUNICATION

#include "common.h"
#include "client_server.h"
#include "contact_list.h"
#include "upush_client.h"

// send a registration msg to server. 
// used both for first registration and heartbeats
void send_reg_msg_to_server();

// try 3 times to send_blocking_lookup_message
// returns 1 if nick was found and buf params are set
// returns 0 on "NICK not REGISTERED", and buf params are not set
// exits program if there was no reply in any of the attempts
int lookup_user_on_server(char * target_nick, char * tmp_nick_buf, struct sockaddr_in * tmp_addr_buf);

// send a lookup request to server, and set it into the buffer params
// returns 1 if nick was found and buf params are set
// returns -1 if nick was not registered
// returns 0 if timeout 
int send_blocking_lookup_message(char * message, char * tmp_nick_buf, struct sockaddr_in * tmp_addr_buf);

#endif