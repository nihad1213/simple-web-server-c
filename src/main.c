#include "socket.h"
#include "bind.h"

int main() {

    int sockfd = create_socket();

    check_socket(&sockfd);
    
    bind_socket(&sockfd);
    
    close_socket(&sockfd);

    return 0;
}