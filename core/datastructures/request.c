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
    reql->resp = NULL;
    return reql;
}

resp_t *createResp(request_t* Req) {
    resp_t *resp_l = (resp_t*)malloc(sizeof(resp_t));
    if(resp_l == NULL) {
        printf("Couldn't create Resp");
        return NULL;
    }

    resp_l->Req = Req;
    resp_l->Headers = NULL;
    resp_l->body = NULL;
    resp_l->status = 0;
    resp_l->reason = NULL;
    return resp_l;
}

void kill_Req(request_t* reql){
    if(reql->Headers == NULL){
        //
    } else {
        ht_destroy(reql->Headers);
    }
    free(reql->resp);
    free(reql);
}

int comp_req(request_t* reql, request_t* reqr){

    int rl_flag = strcmp(reql->req_line, reqr->req_line);
    int ht_flag = ht_compare(reql->Headers, reqr->Headers);

    if(rl_flag == 0 && ht_flag == 0) {
        return 0;
    } else { return 1; }
}