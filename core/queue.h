#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED
#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include "mutex.h"

typedef struct{
    request_t *Req;
    struct node* next;
}node;

typedef struct{
    node *Head;
    node *Tail;
    int size;
    monitor* qlock; /* Monitor for queue access*/
}queue;

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

//initialize the Qs types and its access monitor
queue* CreateQ(){
    queue* new_q = (queue *)malloc(sizeof(queue));
    if(new_q == NULL)
        return NULL;
    new_q->Head = NULL;
    new_q->Tail = NULL;
    new_q->size = 0;
    monitor_init(new_q->qlock,0);
    return new_q;
}


/*
 * Ensure the Q and New node are not null
 * */
void Enqueue(node *new_n,queue *Q){
    if(Q == NULL)
       Q = CreateQ();
    pthread_mutex_lock(&(Q->qlock->lock));
    switch(Q->size){
        case 0:
            Q->Head = new_n;
            Q->Tail = new_n;
            Q->size += 1;
            toggle_monitor(Q->qlock,1);
            break;
        default:
            Q->Tail->next = new_n;
            Q->Tail = new_n;
    }
    pthread_mutex_unlock(&(Q->qlock->lock));
}

node* Dequeue(queue *Q){
    pthread_mutex_lock(&(Q->qlock->lock));
    node* rn;
    switch(Q->size){
        case 0:
            rn = NULL;
            break;
        case 1:
            rn = Q->Head;
            Q->Head = NULL, Q->Tail = NULL;
            Q->size -= 1;
            toggle_monitor(Q->qlock, 0);
            break;
        default:
            rn = Q->Head;
            Q->Head = rn->next;
            Q->size -= 1;
    }
    pthread_mutex_unlock(&(Q->qlock->lock));
    return rn;
}

//Read operation, doesn't require mutually exclusive access
node* peek(queue *Q){
    if(Q->Head == NULL || Q == NULL)
        return NULL;
    return Q->Head;
}


#endif // QUEUE_H_INCLUDED
