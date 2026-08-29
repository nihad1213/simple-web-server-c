#include "bind.h"

int bind_socket(int* sockfd) {
    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(*sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(*sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully bound to port %d\n", PORT);
    return 0;
}