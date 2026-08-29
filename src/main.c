#include <stdio.h>
#include "socket.h"

int main() {

    int sockfd = create_socket();

    check_socket(&sockfd);

    close_socket(&sockfd);

    return 0;
}