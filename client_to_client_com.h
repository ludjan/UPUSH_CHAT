#ifndef CLIENT_TO_CLIENT_COMMUNICATION
#define CLIENT_TO_CLIENT_COMMUNICATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "client_server.h"
#include "contact_list.h"
#include "upush_client.h"


// registers a new ACK received from contact 
// pop a message from the queue (the one that was just ACKed)
// try to send_next_message_to_contact
void register_ack_message(int pkt_number, contact_t * contact);

// this function does all sending of PKTs to other clients
void send_message_to_contact(contact_t * contact, message_t * message);

// try to resend a message to a contact, and perform lookup if necessary
void resend_message_to_contact(contact_t * contact);

// check if there is a message in the send queue
// if so, toggle the PKT number and send_message_to_contact
void send_next_message_to_contact(contact_t * contact);

// iterate over contacts, and see if any have a message we should resend
void resend_pending_messages();

#endif