#include "canon_tpool.h"
#include "../DataStructures/queue.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Parser.h"
#include "../mutex.h"


int THPOOL_ALIVE = 1;


/*PROTOTYPES*/



/*                        TYPE IMPLEMENTATIONS & PROTOTYPES
------------------------------------------------------------------------------------*/

//Creates threads ie; populates the thread pool
static int thinit(thpool_t* pool, threads** threads_p, int id);
//Kills threads
void thkill(threads* thread_p);
//Spurs up the threads functionality
void work(threads* thread_p);

struct threads_p{
    pthread_t threadid;
    int id;
    thpool_t* pool;
};

struct thpool{
    threads **thpool_arr; //Pointer to the array of threads
    queue* job_queue; //Pointer to the queue (Defined in core/queue.h)
    int working; // Number of working threads
    int alive; //Number of alive threads
    pthread_mutex_t poolmutex; //Mutex for the thread pools types
    pthread_cond_t cond; // Conditional variable to control pool activity
};


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
        free(pool);
    }

    for(loopv = 0; loopv<size; loopv++){
        thinit(pool, &(pool)->thpool_arr[loopv], loopv);
    }

    pthread_mutex_init(&(pool->poolmutex), NULL);
    pthread_cond_init(&(pool->cond),NULL);

    while(pool->alive!=size){
        /*
         * Wait for the threads to start up to avoid
         * inconsistencies when assigning jobs
         * 'alive': a constant val == size
         * only constant after thread startup
         * */
    }

    /* Initialize access locks for the pool */

    return pool;

}

void pool_kill(thpool_t* tpool) {
    if(tpool == NULL) return;
    int loopv;
    /*Kill the threads that are active*/
    volatile int size = tpool->alive;
    THPOOL_ALIVE = 0;
    while(tpool->alive) {
        toggle_monitor(tpool->job_queue->qlock, 1);
    }
    for(loopv = 0; loopv<size; loopv++) {
        thkill(tpool->thpool_arr[loopv]);
    }
    free(tpool->thpool_arr);
    free(tpool);
}


/*                            THREAD SPECIFIC FUNCTIONS
  ------------------------------------------------------------------------------------
 * TODO Make thread "work" function agnostic
 */

//Initializes threads 'types'
static int thinit(thpool_t* pool, threads** threads_p, int id){
    *threads_p = (threads *)malloc(sizeof(threads));
    if(*threads_p==NULL)
        return -1;
    (*threads_p)->pool = pool;
    (*threads_p)->id = id;

    pthread_create(&(*threads_p)->threadid, NULL, (void *)work, (*threads_p));
    pthread_detach((*threads_p)->threadid);
    return 0;
}

void hold(int sig_id){
    (void)sig_id;
    int hold_t = 1;
    while(hold_t) {
        sleep(10);
    }
}

void work(threads* threads_p){

    struct sigaction inter; /*Interrupt thread activity and put the thread on hold*/
    sigemptyset(&inter.sa_mask);
    inter.sa_handler = hold;
    /*
     * Assumes an initialized work queue
     * and monitor
     * */

    /*Wait for the queue to be populated*/
    while (THPOOL_ALIVE) {

        pthread_mutex_lock(&(threads_p)->pool->poolmutex);
        threads_p->pool->alive += 1; //increment the alive threads
        pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

        condwait(threads_p->pool->job_queue->qlock);

        pthread_mutex_lock(&(threads_p)->pool->poolmutex);
        (threads_p)->pool->working += 1;
        pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

        node *req = Dequeue((threads_p)->pool->job_queue);
        if(req == NULL)
            condwait(threads_p->pool->job_queue->qlock);
        else{ HTTPMsgParse(req->Req); /*The parse invokes the response builder*/ }

        pthread_mutex_lock(&(threads_p)->pool->poolmutex);
        (threads_p)->pool->working -= 1;
        pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

    }
    pthread_mutex_lock(&(threads_p)->pool->poolmutex);
    (threads_p)->pool->alive -= 1;
    pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

}

void thkill(threads* thread_p) {

    free(thread_p);

}