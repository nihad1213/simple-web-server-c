#include "socket.h"

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket successfully created with descriptor: %d\n", sockfd);
    return sockfd;
}

void check_socket(int* sockfd) {
    if (*sockfd < 0) {
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }
}

int close_socket(int* sockfd) {
    int result = close(*sockfd);
    printf("Socket closed: %d\n", result);
    return result;
}

