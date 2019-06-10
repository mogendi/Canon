#include "canon_tpool.h"
#include "../DataStructures/queue.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "Parser.h"
#include "../mutex.h"

/*                        TYPE IMPLEMENTATIONS
------------------------------------------------------------------------------------*/

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

struct jobq_p{
    queue* Q;
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
    printf("%d", id);
    fflush(stdout);

    pthread_create(&(*threads_p)->threadid, NULL, (void *)work, (*threads_p));
    pthread_detach((*threads_p)->threadid);
    return 0;
}

void work(threads* threads_p){
    /*
     * Assumes an initialized work queue
     * and monitor
     * */

    /*Wait for the queue to be populated*/

    pthread_mutex_lock(&(threads_p)->pool->poolmutex);
    threads_p->pool->alive += 1; //increment the alive threads
    pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

    condwait(threads_p->pool->job_queue->qlock);

    pthread_mutex_lock(&(threads_p)->pool->poolmutex);
    (threads_p)->pool->working += 1;
    pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

    node* req = Dequeue((threads_p)->pool->job_queue);
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
