//
// Created by nelson on 6/9/19.
//

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include "mutex.h"


node* new_node(void *r, node *next){
    node *new_n = (node *)malloc(sizeof(node));
    if(r == NULL){
        printf("Null request: can't create queue node");
        return NULL;
    }
    if(new_n == NULL){
        printf("Queue error");
        return NULL;
    }
    new_n->r = r;
    new_n->next = next;
    return new_n;
}

queue* new_q(){
    queue* nq = (queue *)malloc(sizeof(queue));
    if(nq == NULL)
        exit(1);
    nq->Head = NULL;
    nq->Tail = NULL;
    nq->size = 0;
    nq->qlock = monitor_init(0);
    return nq;
}

void enqueue(void *r, queue *Q){
    node* new_n = new_node(r, NULL);

    if(Q == NULL)
        Q = new_q();
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
            Q->size ++;
    }
    pthread_mutex_unlock(&(Q->rwmutex));
}

void* dequeue(queue *Q){
    node* rn; void* req = NULL;
    int flag=0;
    pthread_mutex_lock(&(Q->rwmutex));
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
    if(rn != NULL)
        req = rn->r;
    free(rn);
    return req;
}

/*
 * Returns 0 if the processes is successful
 * 1 otherwise
 * @Paramas: A - origin queue, B - Target queue,
 * size - number of elements to map*/
int map(queue* A, queue* B, int size) {
    if(A->size < size) {
        printf("Origin queue size too small\n");
        return 1;
    }

    int loopv = 0;

    void* shared_loc[size];

    time_t original = time(NULL), access_time;

    pthread_mutex_lock(&A->rwmutex);

    for (; loopv < (size-1); loopv++) {
        access_time = time(NULL);
        shared_loc[loopv] = dequeue(A);
        if((access_time - original) > 3){
            pthread_mutex_unlock(&A->rwmutex);
            printf("Read too costly, aborting;");
            return loopv;
        }
    }

    pthread_mutex_unlock(&A->rwmutex);

    pthread_mutex_lock(&B->rwmutex);

    for(loopv = 0; loopv < (size-1); loopv++) {
        enqueue(shared_loc[loopv], B);

        access_time = time(NULL);

        if((access_time - original) > 6){
            pthread_mutex_unlock(&B->rwmutex);
            printf("Read/write too costly, aborting;");
            return loopv;
        }
    }
    pthread_mutex_unlock(&B->rwmutex);

    return loopv;
}

int destroy_q(queue* Q) {
    if(Q == NULL) return 0;
    node* cursor = Q->Head;
    while(cursor != NULL) {
        cursor = Q->Head;
        kill_req(cursor->r);
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