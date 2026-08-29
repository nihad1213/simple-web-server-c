#include "socket.h"
#include "bind.h"
#include "listen.h"

int main() {

    int sockfd = create_socket();

    check_socket(&sockfd);

    bind_socket(&sockfd);

    listen_socket(&sockfd);

    close_socket(&sockfd);

    return 0;
}