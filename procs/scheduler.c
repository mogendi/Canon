//
// Created by nelson on 6/5/20.
//

#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

status poll_token(rr_t* rr, handler_t h){
    int ret = 0;
    for(int i = 0; i < RESET; i++){
        if(rr->table[i]->timer_s == 0 || rr->table[i]->cp == NULL)
            continue;
        rr->table[i]->timer_c = clock();
        //If its timed out
        if(((double)(rr->table[i]->timer_c - rr->table[i]->timer_s))/CLOCKS_PER_SEC > (2*CYCLE)) {
            epoll_ctl(rr->aio_handler, EPOLL_CTL_DEL, rr->table[i]->cp->sockfd, &rr->table[i]->ev);
            rr->table[i]->cp->timeout = 1;
            h(rr->table[i]->cp);
            kill_req(rr->table[i]->cp);
            rr->table[i]->cp = NULL;
            rr->table[i]->timer_s = 0;
            rr->table[i]->timer_c = 0;
            rr->slots++;
            ret = 1;
        }
    }
    return ret;
}

rr_t* init_sch(queue* q){
    //round_robin init
    rr_t* rr = malloc(sizeof(rr_t*));
    if(rr == NULL){
        printf("Failed to create rr");
        exit(1);
    }

    rr->table = malloc(RESET* sizeof(request_t*));
    rr->q_point = q;
    rr->slots = 0;

    //fill the rr
    if(q->size < RESET){
        //condwait(q->qlock);
    }
    int i = 0;
    for(; i<RESET; i++) {
        rr->table[i] = NULL;
        rr->table[i] = (tok_t*)malloc(sizeof(tok_t));
        if(rr->table[i] == NULL){
            printf("Failed token creation");
            exit(1);
        }
        if(q->size <= 0){
            rr->table[i]->cp = NULL;
            rr->slots++;
            continue;
        } else {
            rr->table[i]->cp = create_request(*(int *) dequeue(q));
            rr->table[i]->cp->timeout = 0;
        }
    }

    rr->aio_handler = epoll_create1(0);
    if(rr->aio_handler == -1){
        perror("aio init failed");
        exit(-1);
    }
    return rr;
}

status spool(rr_t* rr, handler_t h){
    //Initialize epoll semantics
    struct epoll_event evs[RESET];
    int nfds;
    int tm_total = 0;

    int i = 0, j = 0;
    for(; i<RESET; i++){
        if(rr->table[i]->cp == NULL){
            rr->table[i]->timer_s = 0;
            continue;
        }
        rr->table[i]->ev.events = EPOLLIN;
        rr->table[i]->ev.data.fd = rr->table[i]->cp->sockfd;
        rr->table[i]->ev.data.ptr = rr->table[i]->cp;
        if(epoll_ctl(rr->aio_handler, EPOLL_CTL_ADD, rr->table[i]->cp->sockfd, &rr->table[i]->ev) < 0) {
            perror("epoll_ctl");
            exit(1);
        }
        rr->table[i]->timer_s = clock();
    }

    wait_trigger:
    nfds = epoll_wait(rr->aio_handler, evs, RESET, CYCLE);
    tm_total += CYCLE;
    if(nfds == 0){
        if(rr->q_point->size > 0) {
            printf("Starved\n");
            exit(1);
        } else {
            int k;
            for(k = 0; k < RESET; k++)
                if(rr->table[k]->cp == NULL || rr->table[k]->timer_s == 0)
                    break;
            rr->table[k]->cp = create_request(*(int *) dequeue(rr->q_point));
            rr->table[k]->ev.data.fd = rr->table[k]->cp->sockfd;
            rr->table[k]->ev.data.ptr = rr->table[k]->cp;
            epoll_ctl(rr->aio_handler, EPOLL_CTL_ADD, rr->table[k]->cp->sockfd, &rr->table[k]->ev);
            goto wait_trigger;
        }
    } if(nfds < 0){
        perror("epoll_wait");
        exit(1);
    }
    for (j = 0; j < nfds; ++j) {
        if(rr->table[j]->cp == NULL){
            j += j;
            continue;
        }
        h(rr->table[j]->ev.data.ptr);//Request handler
        request_t* r = rr->table[j]->cp;
        if(rr->table[j]->cp->ka)
            goto keep_alive;
        else {
            goto done_req;
        }
        return_point:
        j = j;
    }
    poll_token(rr, h);

    if(SPOOL)
        goto wait_trigger;
    else {
        goto done;
    }

    keep_alive:
    if(rr->slots > 0){
        if(rr->q_point->size > 0) {
            for(int k = 0 ; k < RESET; k++){
                if(rr->table[k]->cp == NULL){
                    rr->table[k]->cp = create_request(*(int *) dequeue(rr->q_point));
                    epoll_ctl(rr->aio_handler, EPOLL_CTL_ADD, rr->table[k]->cp->sockfd, &rr->table[i]->ev);
                    rr->slots--;
                    rr->table[k]->timer_s = clock();
                }
            }
        }
    }
    goto return_point;


    done_req:
    epoll_ctl(rr->aio_handler, EPOLL_CTL_DEL, rr->table[j]->cp->sockfd, &rr->table[j]->ev);
    kill_req(rr->table[j]->cp);
    rr->table[j]->cp = NULL;
    rr->table[j]->timer_s = 0;
    rr->table[j]->timer_c = 0;
    goto return_point;

    done:
    return 0;
}