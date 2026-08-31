#include "socket.h"
#include "bind.h"
#include "listen.h"
#include "accept.h"

int main() {

    int sockfd = create_socket();

    check_socket(&sockfd);

    bind_socket(&sockfd);

    listen_socket(&sockfd);

    int client_fd = accept_socket(&sockfd);

    close(client_fd);

    close_socket(&sockfd);

    return 0;
}