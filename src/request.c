#include "request.h"

int read_request(int* client_fd, char* buffer) {
    memset(buffer, 0, BUFFER_SIZE);

    ssize_t bytes_read = read(*client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("Read failed");
        close(*client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Received request:\n%s\n", buffer);
    return (int)bytes_read;
}

int parse_request(const char* raw_request, http_request_t* request) {
    memset(request, 0, sizeof(*request));

    if (sscanf(raw_request, "%15s %1023s", request->method, request->path) != 2) {
        return -1;
    }

    return 0;
}
