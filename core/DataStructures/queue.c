//
// Created by nelson on 6/9/19.
//

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include "../mutex.h"


node* CreateNode(request_t *Req, node* next){
    node *new_n = (node *)malloc(sizeof(node));
    if(Req == NULL){
        printf("Null request: can't create queue node");
        return NULL;
    }
    if(new_n == NULL){
        printf("Queue error");
        return NULL;
    }
    new_n->Req = Req;
    new_n->next = next;
    return new_n;
}

queue* CreateQ(){
    queue* new_q = (queue *)malloc(sizeof(queue));
    if(new_q == NULL)
        exit(1);
    new_q->Head = NULL;
    new_q->Tail = NULL;
    new_q->size = 0;
    new_q->qlock = monitor_init(0);
    return new_q;
}

void Enqueue(node *new_n,queue *Q){
    if(Q == NULL)
        Q = CreateQ();
    int flag=0;
    pthread_mutex_lock(&(Q->rwmutex));
    switch(Q->size){
        case 0:
            Q->Head = new_n;
            Q->Tail = new_n;
            Q->size += 1;
            toggle_monitor(Q->qlock, 1);
            break;
        default:
            Q->Tail->next = new_n;
            Q->Tail = new_n;
    }
    pthread_mutex_unlock(&(Q->rwmutex));
}

node* Dequeue(queue *Q){
    pthread_mutex_lock(&(Q->rwmutex));
    node* rn;
    int flag=0;
    switch(Q->size){
        case 0:
            rn = NULL;
            break;
        case 1:
            rn = Q->Head;
            Q->Head = NULL, Q->Tail = NULL;
            Q->size -= 1;
            flag = 0;
            break;
        default:
            rn = Q->Head;
            Q->Head = rn->next;
            Q->size -= 1;
    }
    pthread_mutex_unlock(&(Q->rwmutex));
    toggle_monitor(Q->qlock, flag);
    return rn;
}

int destroy_q(queue* Q) {
    if(Q == NULL) return 0;
    node* cursor;
    while(cursor != NULL) {
        cursor = Q->Head;
        kill_Req(cursor->Req);
        Q->Head = cursor->next;
        free(cursor);
    }
    free(Q->qlock);
    free(Q);
}

node* peek(queue *Q){
    if(Q->Head == NULL || Q == NULL)
        return NULL;
    return Q->Head;
}