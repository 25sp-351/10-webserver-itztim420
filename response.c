#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "response.h"
#include <sys/socket.h>  

void send_response(int client_fd, const char *status, const char *content_type, const char *body, size_t length) {
    char header[1024];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %s\r\nContent-Length: %zu\r\nContent-Type: %s\r\nConnection: close\r\n\r\n",
        status, length, content_type);
    send(client_fd, header, header_len, 0);
    send(client_fd, body, length, 0);
}

void handle_calc(int client_fd, const char *path) {
    char op[8];
    int a, b, result;
    if (sscanf(path, "/calc/%7[^/]/%d/%d", op, &a, &b) == 3) {
        if (strcmp(op, "add") == 0) result = a + b;
        else if (strcmp(op, "mul") == 0) result = a * b;
        else if (strcmp(op, "div") == 0 && b != 0) result = a / b;
        else {
            send_response(client_fd, "400 Bad Request", "text/plain", "Invalid operation or division by zero", 38);
            return;
        }
        char body[64];
        int len = snprintf(body, sizeof(body), "Result: %d\n", result);
        send_response(client_fd, "200 OK", "text/plain", body, len);
    } else {
        send_response(client_fd, "400 Bad Request", "text/plain", "Invalid format", 14);
    }
}

void handle_static(int client_fd, const char *path) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), ".%s", path); // prepend . to stay in current dir

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        send_response(client_fd, "404 Not Found", "text/plain", "File not found", 14);
        return;
    }
    struct stat st;
    fstat(fd, &st);
    size_t filesize = st.st_size;
    char *filedata = malloc(filesize);
    read(fd, filedata, filesize);
    close(fd);

    const char *ext = strrchr(filepath, '.');
    const char *content_type = "application/octet-stream";
    if (ext) {
        if (strcmp(ext, ".png") == 0) content_type = "image/png";
        else if (strcmp(ext, ".html") == 0) content_type = "text/html";
        else if (strcmp(ext, ".txt") == 0) content_type = "text/plain";
    }

    send_response(client_fd, "200 OK", content_type, filedata, filesize);
    free(filedata);
}

void handle_sleep(int client_fd, const char *path) {
    int seconds;
    if (sscanf(path, "/sleep/%d", &seconds) == 1) {
        sleep(seconds);
        char msg[64];
        int len = snprintf(msg, sizeof(msg), "Slept for %d seconds\n", seconds);
        send_response(client_fd, "200 OK", "text/plain", msg, len);
    } else {
        send_response(client_fd, "400 Bad Request", "text/plain", "Invalid sleep path", 18);
    }
}

void handle_request(int client_fd, HttpRequest *req) {
    if (strcmp(req->method, "GET") != 0) {
        send_response(client_fd, "405 Method Not Allowed", "text/plain", "Only GET supported", 18);
        return;
    }
    if (strncmp(req->path, "/calc/", 6) == 0) {
        handle_calc(client_fd, req->path);
    } else if (strncmp(req->path, "/static/", 8) == 0) {
        handle_static(client_fd, req->path);
    } else if (strncmp(req->path, "/sleep/", 7) == 0) {
        handle_sleep(client_fd, req->path);
    } else {
        send_response(client_fd, "404 Not Found", "text/plain", "Unknown path", 12);
    }
}