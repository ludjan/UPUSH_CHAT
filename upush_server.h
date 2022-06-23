#ifndef UPUSH_SERVER
#define UPUSH_SERVER

#include <signal.h>
#include "common.h"
#include "send_packet.h"
#include "server_client_list.h"

int fd;

void signal_handler();

void quit_server();

#endif