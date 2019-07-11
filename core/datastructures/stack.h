#ifndef STACK_H
#define STACK_H

#define atom 2
#define micro 8
#define mini 32
#define tiny 64
#define small 256
#define lim 1024

#include <pthread.h>
#include <zconf.h>
#include <stdio.h>

/* Explicitly defined stack, with a pre defined
    size property. 
    
    The stack data is stored in a table (can be randomly invoked)
    though  the pushes and pops are standard algorithimically
                                                      
                     ----------------
push [data]--->     |                | ---> pop [address at the top]
                     ----------------
                    |                |
                     ---------------- 
                    |                |
                     ----------------      */

typedef struct stack_p stack;

#ifdef __unix__
    typedef u_int32_t status;
    #include <pthread.h>
#endif

#ifdef _WIN32
    typedef unsigned int status;
    typedef unsigned int u_int32_t;
    #include "pthread\pthread.h"
#endif

typedef void** void_list;

struct stack_p {
    void* top; 
    void_list table; /* Data table */
    unsigned pos:10;  /* Pos at data table */
    u_int32_t size; /* Local limit */

    pthread_mutex_t lock; /* Mutext primitives */
};

void* pop(stack* stack_l);

/* Returns the popped data in a list */
void_list pop_n(stack* stack_l, int iter);

status push(stack* stack_l, void* data);

/* Takes pushed data in a list format */
status push_n(stack* stack_l, void_list data, int items);

stack* new_stack(int size);

status destroy(stack* stack_l);

void_list destroy_safe(stack* stack_l);


void read_stack(stack* stack_l, const char* format);

#endif