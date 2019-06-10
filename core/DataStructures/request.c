//
// Created by nelson on 6/9/19.
//
#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

request_t *createRequest(char *MSGi, char *method, char *URL, char *req, hashtable_t *hmap, char *body, int sock_fd){
    request_t *reql = malloc(sizeof(request_t));
    if(reql == NULL){
        printf("MALLOC failed\n");
        return NULL;
    }
    reql->MSG = MSGi;
    reql->req_line = req;
    reql->Headers = hmap;
    reql->body = body;
    reql->sockfd = sock_fd;
    return reql;
}

void kill_Req(request_t* reql){
    if(reql->Headers == NULL){
        //
    } else {
        ht_destroy(reql->Headers);
    }
    free(reql);
}