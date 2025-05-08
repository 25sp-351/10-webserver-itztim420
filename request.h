#ifndef REQUEST_H
#define REQUEST_H

#define MAX_METHOD 8
#define MAX_PATH 1024
#define MAX_VERSION 16

typedef struct {
    char method[MAX_METHOD];
    char path[MAX_PATH];
    char version[MAX_VERSION];
} HttpRequest;

HttpRequest parse_http_request(const char *raw);

#endif