#include "response.h"

static void send_all(int fd, const char* data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = write(fd, data + sent, len - sent);
        if (n <= 0) {
            return;
        }
        sent += (size_t)n;
    }
}

static void send_error(int client_fd, int status_code, const char* status_text) {
    char body[256];
    int body_len = snprintf(body, sizeof(body),
        "<html><body><h1>%d %s</h1></body></html>", status_code, status_text);

    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n",
        status_code, status_text, body_len);

    send_all(client_fd, header, (size_t)header_len);
    send_all(client_fd, body, (size_t)body_len);
}

static const char* get_content_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (ext == NULL) {
        return "text/plain";
    }
    if (strcmp(ext, ".css") == 0) {
        return "text/css";
    }
    if (strcmp(ext, ".js") == 0) {
        return "application/javascript";
    }
    if (strcmp(ext, ".txt") == 0) {
        return "text/plain";
    }
    return "text/html";
}

static int resolve_safe_path(const char* requested_path, char* resolved_out, size_t resolved_size) {
    char root_resolved[PATH_MAX];
    if (realpath(WWW_ROOT, root_resolved) == NULL) {
        return -1;
    }

    char full_path[PATH_MAX];
    char target_resolved[PATH_MAX];

    if (strcmp(requested_path, "/") == 0) {
        static const char* index_files[] = { "/index.html", "/index.php", "/index.pl" };
        size_t count = sizeof(index_files) / sizeof(index_files[0]);
        size_t i;
        for (i = 0; i < count; i++) {
            if (snprintf(full_path, sizeof(full_path), "%s%s", WWW_ROOT, index_files[i]) >= (int)sizeof(full_path)) {
                continue;
            }
            if (realpath(full_path, target_resolved) != NULL) {
                break;
            }
        }
        if (i == count) {
            return -1;
        }
    } else {
        if (snprintf(full_path, sizeof(full_path), "%s%s", WWW_ROOT, requested_path) >= (int)sizeof(full_path)) {
            return -1;
        }
        if (realpath(full_path, target_resolved) == NULL) {
            return -1;
        }
    }

    size_t root_len = strlen(root_resolved);
    if (strncmp(target_resolved, root_resolved, root_len) != 0 ||
        (target_resolved[root_len] != '/' && target_resolved[root_len] != '\0')) {
        return -1;
    }

    if (strlen(target_resolved) >= resolved_size) {
        return -1;
    }
    strcpy(resolved_out, target_resolved);
    return 0;
}

static ssize_t run_interpreter(const char* interpreter, const char* file_path, char* out_buf, size_t out_size) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execlp(interpreter, interpreter, file_path, (char*)NULL);
        _exit(127);
    }

    close(pipefd[1]);

    size_t total = 0;
    ssize_t n;
    while (total < out_size - 1 &&
           (n = read(pipefd[0], out_buf + total, out_size - 1 - total)) > 0) {
        total += (size_t)n;
    }
    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);

    out_buf[total] = '\0';

    if (WIFEXITED(status) && WEXITSTATUS(status) == 127) {
        return -1;
    }

    return (ssize_t)total;
}

static void handle_script(int client_fd, const char* interpreter, const char* resolved_path) {
    static char output[BUFFER_SIZE * 4];

    ssize_t len = run_interpreter(interpreter, resolved_path, output, sizeof(output));
    if (len < 0) {
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }

    const char* body = output;
    ssize_t body_len = len;
    const char* separator = strstr(output, "\r\n\r\n");
    size_t sep_len = 4;
    if (separator == NULL) {
        separator = strstr(output, "\n\n");
        sep_len = 2;
    }
    if (separator != NULL) {
        body = separator + sep_len;
        body_len = len - (body - output);
    }

    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %zd\r\nConnection: close\r\n\r\n",
        body_len);

    send_all(client_fd, header, (size_t)header_len);
    send_all(client_fd, body, (size_t)body_len);
}

static void handle_static_file(int client_fd, const char* resolved_path) {
    FILE* file = fopen(resolved_path, "rb");
    if (file == NULL) {
        send_error(client_fd, 404, "Not Found");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0) {
        fclose(file);
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }

    char* file_buffer = malloc((size_t)file_size);
    if (file_buffer == NULL) {
        fclose(file);
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }

    size_t read_bytes = fread(file_buffer, 1, (size_t)file_size, file);
    fclose(file);

    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n",
        get_content_type(resolved_path), read_bytes);

    send_all(client_fd, header, (size_t)header_len);
    send_all(client_fd, file_buffer, read_bytes);

    free(file_buffer);
}

void handle_request(int client_fd, const http_request_t* request) {
    char resolved_path[PATH_MAX];
    if (resolve_safe_path(request->path, resolved_path, sizeof(resolved_path)) != 0) {
        send_error(client_fd, 404, "Not Found");
        return;
    }

    const char* ext = strrchr(resolved_path, '.');

    if (ext != NULL && strcmp(ext, ".php") == 0) {
        handle_script(client_fd, "php", resolved_path);
        return;
    }

    if (ext != NULL && strcmp(ext, ".pl") == 0) {
        handle_script(client_fd, "perl", resolved_path);
        return;
    }

    handle_static_file(client_fd, resolved_path);
}
