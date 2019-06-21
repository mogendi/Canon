#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED
#include "request.h"
#include "../mutex.h"

typedef struct node_p node;
typedef struct queue_p queue;

struct node_p{
    request_t *Req;
    node* next;
};

struct queue_p{
    node *Head;
    node *Tail;
    int size;
    monitor* qlock; /* Monitor for queue access*/
    pthread_mutex_t rwmutex; /*Mutex lock for the queues types*/
};

node* CreateNode(request_t *Req, node* next);

//initialize the Qs types and its access monitor
queue* CreateQ();

/*
 * Ensure the Q and New node are not null
 * */
void Enqueue(request_t* Req,queue *Q);

request_t* Dequeue(queue *Q);

/*
 * Maps a section(of specified size) of one queue
 * to another queue
 * */
int map(queue* A, queue* B, int size);

int destroy_q(queue* Q);

//Read operation, doesn't require mutually exclusive access
node* peek(queue *Q);


#endif // QUEUE_H_INCLUDED
