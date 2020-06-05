#ifndef ROUNDROBIN_H_INCLUDED
#define ROUNDROBIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "datastructures/queue.h"

#define rr_size 5;

typedef struct rr_p rr_t;
typedef request_t* token_t;

struct rr_p{
    request_t* table[5];
    token_t tok;
    queue* q_point;
};

#endif // ROUNDROBIN_H_INCLUDED
