//
// Created by nelson on 5/2/19.
//

#ifndef CANON_MUTEX_H
#define CANON_MUTEX_H

#include <pthread.h>

enum bool {true, false};

typedef struct monitor_p monitor;
typedef struct semaphore semaphore_t;
typedef struct bin_sem bin_sem_t;

struct monitor_p{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int val;
};

struct semaphore{
    pthread_mutex_t lock;
    int sem;
};

struct bin_sem {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    enum bool bin;
};

monitor* monitor_init(int val);

void condwait(monitor* m);

/*
 * These types are subject to parallel access issues
 * there require serialization
 * */

void toggle_monitor(monitor *m, int val);

void toggle_bsem(struct bin_sem* bs, enum bool b);

void inc_sem(semaphore_t* sem);

void dec_sem(semaphore_t* sem);

#endif //CANON_MUTEX_H
