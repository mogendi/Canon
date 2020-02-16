#ifndef REQUEST_H_INCLUDED
#define REQUEST_H_INCLUDED

#include "datastructures/hashmap.h"
#include <time.h>

/*Holds the implementation for both request and response types*/
typedef struct request request_t;
typedef struct resp resp_t;

struct request
{
    int method;

    char *body;

    int sockfd;

    time_t secs;
    resp_t* resp;

    /*Parse literals*/
    char *MSG;

    char* req_start;
    char* req_end; //request line end
    char* method_end;
    char* uri_start;
    char* uri_end;

    char* schema_start;
    char* schema_end;
    char* host_start;
    char* host_end;
    char* port_start;
    char* port_end;

    char* args_start;

    int http_minor;
    int http_major;
    int ver;

    unsigned has_query : 1;
    unsigned has_fragment : 1;

    int complex_uri;
    int plus_in_uri;
    int quoted_uri;

    char* headers_start;
    char* headers_end;
    hashtable_t *Headers;

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
