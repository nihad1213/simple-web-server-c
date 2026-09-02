#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define MAX_PATH_LEN 1024

typedef struct {
    char method[16];
    char path[MAX_PATH_LEN];
} http_request_t;

int read_request(int* client_fd, char* buffer);
int parse_request(const char* raw_request, http_request_t* request);
