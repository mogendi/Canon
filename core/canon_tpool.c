#include "canon_tpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <bits/sigaction.h>
#include "queue.h"


void* threadfluff(threads* thread_p);


/*                        GENERAL POOL FUNCTIONS
------------------------------------------------------------------------------------*/
thpool_t* pool_init(int size, queue* jobQ){

    int loopv;

    if(size <= 0){
        return NULL;
    }

    //Initialize the thread pool
    thpool_t* pool;
    if((pool = (thpool_t *)malloc(sizeof(thpool_t))) == NULL)
        return NULL;
    pool->alive = 0;
    pool->working = 0;
    pool->job_queue = jobQ;
    pthread_cond_init(&(pool->cond), NULL);

    //Initialize the threads
    pool->thpool_arr = (threads**)malloc(size * sizeof(threads));
    if(pool->thpool_arr == NULL){
        printf("Fatal init error: Thread pool startup");
        destroy_pool(pool);
        free(pool);
    }
    for(loopv = 0; loopv<size; loopv++){
        thinit(pool, &(pool->thpool_arr[loopv]), loopv);
    }

    /* Initialize access locks for the Queue & and pool */
    pthread_mutex_init(&(pool->job_queue->qlock->lock), NULL);
    pthread_mutex_init(&(pool->poolmutex), NULL);
    pthread_cond_init(&(pool->job_queue->qlock->cond),NULL);
    pthread_cond_init(&(pool->cond),NULL);

}


/*                            THREAD SPECIFIC FUNCTIONS
  ------------------------------------------------------------------------------------ */
static int thinit(thpool_t* pool, threads** threads_p, int id){
    *threads_p = (threads *)malloc(sizeof(threads));
    (*threads_p)->pool = pool;
    (*threads_p)->id = id;

    pthread_create(&(*threads_p)->threadid, NULL, (void *)threadfluff, (*threads_p));
    return 0;
}

void handler(int sig){

}

void* threadfluff(threads* thread_p){
    //Setup signal for communication
    struct sigaction handle;
    sigemptyset(&handle.sa_mask);
    handle.sa_flags = 0;
    handle.__sigaction_handler.sa_handler = handler;
    if(sigaction(SIGUSR1, &handle, NULL) == -1)
        printf("Fatal Error: Cant build the signal handler");

    pthread_mutex_lock(thread_p->pool->poolmutex);
    thread_p->pool->alive += 1; //increment the alive threads
    pthread_mutex_unlock(thread_p->pool->poolmutex);
}