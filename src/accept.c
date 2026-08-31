#include "accept.h"

int accept_socket(int* sockfd) {
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);

    int client_fd = accept(*sockfd, (struct sockaddr*)&client_address, &addrlen);
    if (client_fd < 0) {
        perror("Accept failed");
        close(*sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Accepted connection with descriptor: %d\n", client_fd);
    return client_fd;
}
