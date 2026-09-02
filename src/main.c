#include "socket.h"
#include "bind.h"
#include "listen.h"
#include "accept.h"
#include "request.h"
#include "response.h"

int main() {

    int sockfd = create_socket();

    check_socket(&sockfd);

    bind_socket(&sockfd);

    listen_socket(&sockfd);

    while (1) {
        int client_fd = accept_socket(&sockfd);

        char buffer[BUFFER_SIZE];
        read_request(&client_fd, buffer);

        http_request_t request;
        if (parse_request(buffer, &request) == 0) {
            handle_request(client_fd, &request);
        }

        close(client_fd);
    }

    return 0;
}