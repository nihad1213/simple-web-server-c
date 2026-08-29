#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int create_socket();
void check_socket(int* sockfd);
int close_socket(int* sockfd);