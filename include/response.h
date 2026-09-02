#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>

#include "request.h"

#define WWW_ROOT "www"

void handle_request(int client_fd, const http_request_t* request);
