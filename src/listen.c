#include "listen.h"

int listen_socket(int* sockfd) {
    if (listen(*sockfd, SOMAXCONN) < 0) {
        perror("Listen failed");
        close(*sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Socket is now listening for connections\n");
    return 0;
}
