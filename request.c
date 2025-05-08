#include <string.h>
#include "request.h"
#include <stdio.h>  

HttpRequest parse_http_request(const char *raw) {
    HttpRequest req;
    memset(&req, 0, sizeof(req));
    sscanf(raw, "%s %s %s", req.method, req.path, req.version);
    return req;
}
