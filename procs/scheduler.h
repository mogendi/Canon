//
// Created by nelson on 6/5/20.
//

#ifndef CANON_SCHEDULER_H
#define CANON_SCHEDULER_H

/* Round Robin + async io + timing dynamics that manage
 * the data being processed by a processes thread pool.
 * */


#include "../http/handler.h"
#include "../core/queue.h"
#include <time.h>
#include <sys/epoll.h>
#define SPOOL 1

#define CYCLE 1     //milliseconds
#define PROC_TM 0.2 //milliseconds
#define RESET 5

typedef struct rr_p rr_t;
typedef struct tok tok_t;
typedef tok_t** table_t;
typedef status (*handler_t)(request_t* data);

struct tok{
    request_t* cp; //current process
    clock_t timer_s;
    clock_t timer_c;
    struct epoll_event ev;
};

struct rr_p{
    table_t table;
    queue* q_point;
    unsigned int aio_handler; //epoll fd
    int slots; //running count of empty slots
};


rr_t* init_sch(queue* q);
status spool(rr_t* rr, handler_t h);

#endif //CANON_SCHEDULER_H
