#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include "canon_tpool.h"
#include "../mutex.h"

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

    while(pool->working!=size){
        /*
         * Wait for the threads to start up to avoid
         * inconsistencies when assigning jobs
         * 'alive': a constant val == size
         * only constant after thread startup
         * */
    }

    /* Initialize access locks for the pool */
    pthread_mutex_init(&(pool->poolmutex), NULL);
    pthread_cond_init(&(pool->cond),NULL);

    return pool;

}


/*                            THREAD SPECIFIC FUNCTIONS
  ------------------------------------------------------------------------------------ */


//Prototypes
void handler(int sig, threads** threads_p);

//Initializes threads 'types' builds the signal handler
static int thinit(thpool_t* pool, threads** threads_p, int id){
    *threads_p = (threads *)malloc(sizeof(threads));
    if(*threads_p==NULL)
        return -1;
    (*threads_p)->pool = pool;
    (*threads_p)->id = id;

    pthread_create(&(*threads_p)->threadid, NULL, (void *)work, (*threads_p));
    struct sigaction handle;
    sigemptyset(&handle.sa_mask);
    handle.sa_flags = 0;
    handle.sa_handler = handler;
    if(sigaction(SIGUSR1, &handle, NULL) == -1) {
        printf("Fatal Error: Cant build the signal handler");
        return -1;
    }
    pthread_mutex_lock(&((*threads_p)->pool->poolmutex));
    (*threads_p)->pool->alive += 1; //increment the alive threads
    pthread_mutex_unlock(&((*threads_p)->pool->poolmutex));
    return 0;
}

void work(threads** threads_p){
    /*
     * Assumes an initialized work queue
     * and monitor
     * */

    /*Wait for the queue to be populated*/
    condwait((*threads_p)->pool->job_queue->qlock);

    pthread_mutex_lock(&(*threads_p)->pool->poolmutex);
    (*threads_p)->pool->working += 1;
    pthread_mutex_unlock(&(*threads_p)->pool->poolmutex);

    node* req = Dequeue((*threads_p)->pool->job_queue);
    HTTPMsgParse(req->Req); /*The parse invokes the response builder*/

}

//Kills the threads on SIGUSR1
void handler(int sig, threads** threads_p){
    if(sig==SIGUSR1){
        pthread_mutex_lock(&(*threads_p)->pool->poolmutex);
        (*threads_p)->pool->alive -= 1;
        pthread_mutex_unlock(&(*threads_p)->pool->poolmutex);
        free(threads_p);
    }
}
