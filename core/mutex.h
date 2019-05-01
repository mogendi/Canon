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
};

struct semaphore{
    pthread_mutex_t lock;
    int sem;
};

struct bin_sem {
    pthread_mutex_t lock;
    enum bool bin;
};

#endif //CANON_MUTEX_H
