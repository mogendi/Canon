//
// Created by nelson on 6/9/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mutex.h"

monitor* monitor_init(int val){

    monitor* m = (monitor *)malloc(sizeof(monitor));
    if(m == NULL){
        printf("MONITOR initialization failed");
        return NULL;
    }

    if(1<val||val<0){
        printf("Error: Can't initialize Mutex types");
        exit(1);
    }
    pthread_mutex_init(&m->lock, NULL);
    pthread_cond_init(&m->cond, NULL);
    m->val = val;
    if(val == 1)
        pthread_cond_signal(&m->cond);

    return m;
}

void condwait(monitor* m){
    pthread_mutex_lock(&m->lock);
    //Threads always enter wait whenever the JobQ is empty
    while(m->val != 1){
        pthread_cond_wait(&m->cond, &m->lock);
    }
    pthread_mutex_unlock(&m->lock);

}

void toggle_monitor(monitor *m, int val){
    pthread_mutex_lock(&m->lock);
    m->val = val;
    if(val == 1)
        pthread_cond_broadcast(&m->cond);
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

