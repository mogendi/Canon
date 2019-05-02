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

void monitor_init(monitor* m, int val){
    if(1<val||val<0){
        printf("Error: Can't initialize Mutex types");
        exit(1);
    }
    pthread_mutex_init(&m->lock, NULL);
    pthread_cond_init(&m->cond, NULL);
    m->val = val;
    if(val == 1)
        pthread_cond_signal(&m->cond);
}

void condwait(monitor* m){
    pthread_mutex_lock(&m->lock);
    //Threads always enter wait whenever the JobQ is empty
    while(m->val != 1){
        pthread_cond_wait(&m->cond, &m->lock);
    }
    pthread_mutex_unlock(&m->lock);

}

/*
 * These types are subject to parallel access issues
 * there require serialization
 * */

void toggle_monitor(monitor *m, int val){
    pthread_mutex_lock(&m->lock);
    m->val = val;
    if(val == 1)
        pthread_cond_signal(&m->cond);
    pthread_mutex_unlock(&m->lock);
}

void toggle_bsem(struct bin_sem* bs, enum bool b){
    pthread_mutex_lock(&bs->lock);
    bs->bin = b;
    if(b == true)
        pthread_cond_signal(&bs->cond);
    pthread_mutex_unlock(&bs->lock);
}

void inc_sem(semaphore_t* sem){
    pthread_mutex_lock(&sem->lock);
    sem->sem += 1;
    pthread_mutex_unlock(&sem->lock);
}

void dec_sem(semaphore_t* sem){
    pthread_mutex_lock(&sem->lock);
    sem->sem -= 1;
    pthread_mutex_unlock(&sem->lock);
}

#endif //CANON_MUTEX_H
