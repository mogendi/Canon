#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED
#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include "canon_tpool.h"

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
        return NULL;
    new_q->Head = NULL;
    new_q->Tail = NULL;
    new_q->size = 0;
    return new_q;
}

void Enqueue(node *new_n, request_t *Req, node* next,queue *Q){
    queue* new_q;
    if(Q == NULL)
       new_q = CreateQ();
    if(new_n == NULL)
     new_n = CreateNode(Req, next);
    if(Q->Head == NULL){
        Q->Head = new_n;
        Q->Tail = new_n;
        Q->size += 1;
        return;
    }
    Q->Tail->next = new_n;
    Q->Tail += 1;
}

node* Dequeue(queue *Q){
    if(Q->Tail == NULL || Q == NULL){
        return NULL;
    }
    node* cursor = Q->Head;
    Q->Head = cursor->next;
    return cursor;
}

node* peek(queue *Q){
    if(Q->Head == NULL || Q == NULL)
        return NULL;
    return Q->Head;
}


#endif // QUEUE_H_INCLUDED
