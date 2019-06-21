#ifndef REQUEST_H_INCLUDED
#define REQUEST_H_INCLUDED

#include "hashmap.h"
#include <time.h>


/*Holds the implementation for both request and response types*/


typedef struct request request_t;
typedef struct resp resp_t;

struct request
{
    char *MSG;
    char *method;
    char *URL;
    char *req_line;
    hashtable_t *Headers;
    char *body;
    int sockfd;
    time_t secs;
    resp_t* resp;
};


struct resp {
    request_t* Req;
    int status;
    char* reason;
    hashtable_t *Headers;
    char* body;
};

request_t *createRequest(int sock_fd);

resp_t *createResp(request_t* Req);

void kill_Req(request_t* reql);

int comp_req(request_t* reql, request_t* reqr);

#endif // REQUEST_H_INCLUDED
