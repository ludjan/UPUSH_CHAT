#include "common.h"
#include "send_packet.h"

void test_send(int fd, char * msg, struct sockaddr_in * server_addr) {
     
    int rc;

    rc = send_packet(
        fd, 
        msg,
        strlen(msg), 
        0,
        (struct sockaddr*) server_addr,
        sizeof(struct sockaddr_in)
    );
    if(check_error_perror(rc, "send")) exit(EXIT_FAILURE);
    printf("%s\n", msg);
}

void test_recv(int fd, char buf []) {

    int rc;

    rc = recv(fd, buf, BUFSIZE, 0);
    if(check_error_perror(rc, "recv")) exit(EXIT_FAILURE);

    buf[rc] = 0;
    printf("%s\n", buf);
}

int main () {
    
    int fd;
    
    struct sockaddr_in server_addr;
    struct sockaddr_in my_addr;
    
    char buf [BUFSIZE];
    char * msg;

    set_loss_probability(0);

    create_IPv4_UDP_socket(&fd);

    // 127.0.0.1
    set_values_in_IPv4_addr(&my_addr, "127.0.0.1", 50000);
    bind_sockaddr_to_fd(fd, &my_addr);


    set_values_in_IPv4_addr(&server_addr, "127.0.0.1", 2149);

    // REG
    msg = "PKT 0 REG ludvig";
    test_send(fd, msg, &server_addr);
    test_recv(fd, buf);

    // REG
    msg = "PKT 11 REG gustav";
    test_send(fd, msg, &server_addr);
    test_recv(fd, buf);

    // REG
    msg = "PKT 100 REG jansson";
    test_send(fd, msg, &server_addr);
    test_recv(fd, buf);

    // LOOKUP
    msg = "PKT 5 LOOKUP ludvig";
    test_send(fd, msg, &server_addr);
    test_recv(fd, buf);

    // LOOKUP
    msg = "PKT 55 LOOKUP gustav";
    test_send(fd, msg, &server_addr);
    test_recv(fd, buf);

    // LOOKUP
    msg = "PKT 555 LOOKUP jansson";
    test_send(fd, msg, &server_addr);
    test_recv(fd, buf);

    return EXIT_SUCCESS;

}