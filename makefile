# FLAGS
CFLAGS = -g -std=gnu11 -Wall -Wextra
VALGRIND_FLAGS = --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full

# executables
MAIN_BIN = upush_server upush_client 
TESTS_BIN = upush_client_test test_blocked_nick_list test_contacts_and_messages test_contact_iterator

# the base list, with iterator
BASE_LINKED_LIST_OBJ = base_linked_list.o

# lists and their data
MESSAGE_LIST_OBJ = message_list.o message.o
CONTACT_LIST_OBJ = contact_list.o contact.o
BLOCKED_NICK_LIST_OBJ = blocked_nick_list.o blocked_nick.o
SERVER_CLIENT_LIST_OBJ = server_client_list.o server_client.o

# objects separated per area of use
CLIENT_OBJ = client_to_server_com.o $(CONTACT_LIST_OBJ) $(MESSAGE_LIST_OBJ) $(BLOCKED_NICK_LIST_OBJ) client_server.o client_to_client_com.o time_functions.o $(BASE_LINKED_LIST_OBJ)
SERVER_OBJ = $(SERVER_CLIENT_LIST_OBJ) $(BASE_LINKED_LIST_OBJ)
COMMON_OBJ = common.o send_packet.o

ALL_OBJ = $(CLIENT_OBJ) $(SERVER_OBJ) $(COMMON_OBJ)

all: $(MAIN_BIN) $(TESTS_BIN)

#################################################

# run normally with some test params
run_server: upush_server 
	./upush_server 2149 10

run_client_alpha: upush_client 
	./upush_client alpha 127.0.0.1 2149 5 10

run_client_beta: upush_client 
	./upush_client beta 127.0.0.1 2149 8 20

run_client_gamma: upush_client 
	./upush_client gamma 127.0.0.1 2149 4 15

# run valgrind for testing
run_server_valgrind: upush_server 
	valgrind $(VALGRIND_FLAGS) ./upush_server 2149 10

run_client_alpha_valgrind: upush_client 
	valgrind $(VALGRIND_FLAGS) ./upush_client alpha 127.0.0.1 2149 5 10

run_client_beta_valgrind: upush_client 
	valgrind $(VALGRIND_FLAGS) ./upush_client beta 127.0.0.1 2149 8 20

run_client_gamma_valgrind: upush_client 
	valgrind $(VALGRIND_FLAGS) ./upush_client gamma 127.0.0.1 2149 4 15

#################################################
# main executables
upush_server: upush_server.o $(COMMON_OBJ) $(SERVER_OBJ)
	gcc $(CFLAGS) upush_server.o $(COMMON_OBJ) $(SERVER_OBJ) -o upush_server 

upush_client: upush_client.o $(COMMON_OBJ) $(CLIENT_OBJ)
	gcc $(CFLAGS) upush_client.o $(COMMON_OBJ) $(CLIENT_OBJ) -o upush_client 

# test executables
upush_client_test: upush_client_test.c $(COMMON_OBJ)
	gcc $(CFLAGS) upush_client_test.c $(COMMON_OBJ) -o upush_client_test

test_blocked_nick_list: test_blocked_nick_list.c $(BASE_LINKED_LIST_OBJ) $(BLOCKED_NICK_LIST_OBJ) $(COMMON_OBJ)
	gcc $(CFLAGS) test_blocked_nick_list.c $(BASE_LINKED_LIST_OBJ) $(BLOCKED_NICK_LIST_OBJ) $(COMMON_OBJ) -o test_blocked_nick_list

test_contacts_and_messages: test_contacts_and_messages.c $(BASE_LINKED_LIST_OBJ) $(CONTACT_LIST_OBJ) $(MESSAGE_LIST_OBJ) $(COMMON_OBJ)
	gcc $(CFLAGS) test_contacts_and_messages.c $(BASE_LINKED_LIST_OBJ) $(CONTACT_LIST_OBJ) $(MESSAGE_LIST_OBJ) $(COMMON_OBJ) -o test_contacts_and_messages

test_contact_iterator: test_contact_iterator.c $(BASE_LINKED_LIST_OBJ) $(CONTACT_LIST_OBJ) $(MESSAGE_LIST_OBJ) $(COMMON_OBJ)
	gcc $(CFLAGS) test_contact_iterator.c $(BASE_LINKED_LIST_OBJ) $(CONTACT_LIST_OBJ) $(MESSAGE_LIST_OBJ) $(COMMON_OBJ) -o test_contact_iterator


#################################################
# executable objects
upush_server.o: upush_server.c
	gcc $(CFLAGS) -c upush_server.c

upush_client.o: upush_client.c upush_client.h
	gcc $(CFLAGS) -c upush_client.c

# upush_client_test.o: upush_client_test.c
# 	gcc $(CFLAGS) -c upush_client_test.c

#################################################
# data structure for client

time_functions.o: time_functions.c time_functions.h
	gcc $(CFLAGS) -c time_functions.c

# the client's server object
client_server.o: client_server.c client_server.h
	gcc $(CFLAGS) -c client_server.c

# communications between client and server
client_to_server_com.o: client_to_server_com.c client_to_server_com.h
	gcc $(CFLAGS) -c client_to_server_com.c

# communication between clients
client_to_client_com.o: client_to_client_com.c client_to_client_com.h
	gcc $(CFLAGS) -c client_to_client_com.c

# contacts
contact.o: contact.c contact.h
	gcc $(CFLAGS) -c contact.c
contact_list.o: contact_list.c contact_list.h
	gcc $(CFLAGS) -c contact_list.c

# messages
message.o: message.c message.h
	gcc $(CFLAGS) -c message.c
message_list.o: message_list.c message_list.h
	gcc $(CFLAGS) -c message_list.c

# blocked nicks
blocked_nick_list.o: blocked_nick_list.c blocked_nick_list.h
	gcc $(CFLAGS) -c blocked_nick_list.c
blocked_nick.o: blocked_nick.c blocked_nick.h
	gcc $(CFLAGS) -c blocked_nick.c

#################################################
# data structure for server

server_client.o: server_client.c server_client.h
	gcc $(CFLAGS) -c server_client.c

server_client_list.o: server_client_list.c server_client_list.h
	gcc $(CFLAGS) -c server_client_list.c

#################################################
# common data structure 

base_linked_list.o: base_linked_list.c base_linked_list.h
	gcc $(CFLAGS) -c base_linked_list.c

################################################
# helpers

common.o: common.c common.h
	gcc $(CFLAGS) -c common.c

send_packet.o: send_packet.c send_packet.h
	gcc $(CFLAGS) -c send_packet.c

################################################
# clean
clean:
	rm -f $(MAIN_BIN) $(TESTS_BIN) $(ALL_OBJ) *.o