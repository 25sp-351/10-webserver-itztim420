#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include "request.h"
#include "response.h"

#define DEFAULT_PORT 80

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);
    char buffer[4096];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer)-1, 0);
    if (n <= 0) { close(client_fd); return NULL; }
    buffer[n] = '\0';
    HttpRequest req = parse_http_request(buffer);
    handle_request(client_fd, &req);
    close(client_fd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    if (argc == 3 && strcmp(argv[1], "-p") == 0)
        port = atoi(argv[2]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 10);

    printf("Listening on port %d...\n", port);
    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_fd);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}