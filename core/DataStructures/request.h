#ifndef REQUEST_H_INCLUDED
#define REQUEST_H_INCLUDED

#include "hashmap.h"

struct request
{
    char *MSG;
    char *method;
    char *URL;
    char *req_line;
    hashtable_t *Headers;
    char *body;
    int sockfd;
}request;

typedef struct request request_t;

request_t *createRequest(char *MSGi, char *method, char *URL, char *req, hashtable_t *hmap, char *body, int sock_fd);

void kill_Req(request_t* reql);
#endif // REQUEST_H_INCLUDED