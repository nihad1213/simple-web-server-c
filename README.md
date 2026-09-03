# simple-web-server-c

A small HTTP server written in C from scratch, using only POSIX sockets - no external web server libraries. Built as a learning project to understand what actually happens under the hood between `socket()` and a browser rendering a page.

## Features

- Raw TCP socket setup: `socket()` → `bind()` → `listen()` → `accept()`
- Parses the HTTP request line (method + path)
- Serves static files from `www/` with correct `Content-Type` headers (`.html`, `.css`, `.js`, `.txt`, ...)
- Directory traversal protection - resolves every requested path with `realpath()` and rejects anything that escapes the `www/` root
- Executes `.php` and `.pl` files as CGI-style scripts (via `fork`/`exec`, piping the interpreter's stdout back as the response body)
- Defaults `/` to `index.html`, `index.php`, or `index.pl`, in that order
- Basic error responses (404, 500)

## Project layout

```
include/    Header files (declarations)
src/        Implementation files
www/        Served content (HTML, PHP, Perl)
CMakeLists.txt
```

| File | Responsibility |
|---|---|
| `socket.c/h` | Create and close the listening socket |
| `bind.c/h` | Bind the socket to a port (`8080`) on all interfaces |
| `listen.c/h` | Put the socket into a passive, listening state |
| `accept.c/h` | Accept a queued connection, returning a per-client file descriptor |
| `request.c/h` | Read raw bytes off a client connection and parse the method/path |
| `response.c/h` | Resolve the request path safely, serve static files or run scripts, send the HTTP response |
| `main.c` | Wires everything together in an accept → read → parse → respond → close loop |

## Building

Requires CMake 3.31+ and a C compiler with C23 support.

```bash
mkdir -p build && cd build
cmake ..
make
```

## Running

```bash
./build/simple-web-server
```

Then visit `http://localhost:8080` in a browser, or:

```bash
curl http://localhost:8080/
```

## Scope

This is intentionally a learning-focused, minimal implementation, not a production server:

- Single-threaded and iterative - handles one client at a time, fully serving each request before accepting the next
- Only reads/parses the HTTP request line; headers and request bodies are ignored
- No HTTPS, keep-alive connections, or chunked transfer encoding
