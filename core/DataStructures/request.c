//
// Created by nelson on 6/9/19.
//
#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include <time.h>

request_t *createRequest(int sock_fd){
    request_t *reql = (request_t*)malloc(sizeof(request_t));
    if(reql == NULL){
        printf("Couldn't create Request failed\n");
        return NULL;
    }

    reql->MSG = NULL;
    reql->req_line = NULL;
    reql->Headers = NULL;
    reql->body = NULL;
    reql->sockfd = sock_fd;
    reql->secs = time(NULL)%3600;
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

int comp_req(request_t* reql, request_t* reqr){

    int rl_flag = strcmp(reql->req_line, reqr->req_line);
    int ht_flag = ht_compare(reql->Headers, reqr->Headers);

    if(rl_flag == 0 && ht_flag == 0) {
        return 0;
    } else { return 1; }
}