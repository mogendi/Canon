//
// Created by nelson on 6/9/19.
//
#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hashmap.h"
#include <time.h>

request_t *create_request(int sock_fd){
    request_t *r = malloc(sizeof(request_t));
    if(r == NULL){
        printf("Couldn't create Request failed\n");
        return NULL;
    }
    args_t* args;
    if((args = (args_t*)malloc(sizeof(args_t))) == NULL)
        return NULL;
    r->args = args;

    chunk_t* chunks;
    if((chunks = (chunk_t*)malloc(sizeof(chunk_t))) == NULL)
        return NULL;
    r->chunks = chunks;
    r->chunks->ext = (exts_t*)malloc(sizeof(exts_t));
    r->chunks->ext->query = NULL;
    r->trailers = 0;
    r->chunks->chunk_size = 0;

    r->MSG = NULL;
    r->Headers = ht_create(20);
    r->body = NULL;
    r->sockfd = sock_fd;
    r->resp = NULL;
    r->bl = 0;
    return r;
}

void kill_req(request_t *r){
    if(r->Headers != NULL)
        ht_destroy(r->Headers);
    close(r->sockfd);
    free(r->resp);

    if(r->chunks != NULL) {
        int nc = 1;
        exts_t *e = r->chunks->ext, *ec;
        while (nc) {
            if (e->ext == NULL) {
                free(e->query_arg);
                free(e->query);
                free(e);
                nc = 0;
                continue;
            }
            ec = e;
            e = e->ext;
            free(ec->query_arg);
            free(ec->query);
            free(ec);
        }
        free(r->chunks->body);
    }

    if(r->args != NULL) {
        int na = 1;
        args_t *a = r->args, *ac;
        while (na) {
            if (a->chain == NULL) {
                free(a->query);
                free(a->query_arg);
                free(a);
                na = 0;
                continue;
            }
            ac = a;
            a = a->chain;
            free(ac->query_arg);
            free(ac->query);
            free(ac);
        }
    }


    free(r);
}