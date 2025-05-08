#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"

void handle_request(int client_fd, HttpRequest *req);

#endif
