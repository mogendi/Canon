#ifndef REQUEST_H_INCLUDED
#define REQUEST_H_INCLUDED

#include "hashmap.h"
#include <time.h>

/*Holds the implementation for both request and response types*/
typedef struct request request_t;
typedef struct resp resp_t;
typedef struct args args_t;
typedef struct chunk chunk_t;
typedef struct exts exts_t;

struct request
{

    int sockfd;
    unsigned ka : 1;

    /*Parse literals*/
    char *MSG;

    int method;
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
    char* args_end;

    int http_minor;
    int http_major;
    int ver;

    unsigned has_query : 1;
    unsigned has_fragment : 1;

    int complex_uri;

    args_t* args;

    char* headers_start;
    char* headers_end;
    hashtable_t *Headers;

    char *body;
    chunk_t* chunks;
    unsigned trailers:1;
    char* trailers_start;
    char* trailers_end;

    char* resp;
};


struct resp {
    request_t* Req;
    int status;
    char* reason;
    hashtable_t *Headers;
    char* body;
};

struct args{
    char *lvalue;
    char *query;
    char *query_arg; //arguments/fragment/singular values
    int len;
    args_t* chain; //children (condensed)
};

struct chunk{
    char* body;
    int chunk_size;
    exts_t* ext;
};

struct exts{
    char* query;
    char* query_arg;
    exts_t* ext;
};

request_t *createRequest(int sock_fd);

resp_t *createResp(request_t* Req);

void kill_Req(request_t* reql);

int comp_req(request_t* reql, request_t* reqr);

#endif // REQUEST_H_INCLUDED
