#include "canon_tpool.h"
#include "../DataStructures/queue.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include "../mutex.h"


int THPOOL_ALIVE = 1;


/*PROTOTYPES & IMPL*/

funcs* create_funcs(callback f1, callback f2) {
    funcs* fps = (funcs*)malloc(sizeof(funcs));
    if(fps != NULL) {
        fps->f1 = f1;
        fps->f2 = f2;
        return fps;
    }
}



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

/*                        GENERAL POOL FUNCTIONS
------------------------------------------------------------------------------------*/

thpool_t* pool_init(callback f1, callback f2){

    int size = get_nprocs();

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
    pool->job_queue = CreateQ();
    pool->size = size;

    //Initialize the threads
    pool->thpool_arr = (threads**)malloc((size * sizeof(threads))+1);
    if(pool->thpool_arr == NULL){
        printf("Fatal init error: Thread pool startup");
        free(pool);
    }

    for(loopv = 0; loopv<size; loopv++){
        thinit(pool, &pool->thpool_arr[loopv], loopv);
    }

    pool->work_f = create_funcs(f1, f2);

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

int poll(thpool_t* tpool) {
    if(tpool == NULL)
        return -1;
    printf("THREAD POOL: \n");
    printf("\t THREADS: \n");
    printf("\t\t[");
    printf("\t\t\t   %ld", tpool->thpool_arr[0]->threadid);
    int loopv;
    for(loopv = 0; loopv < tpool->size-2; loopv++) {
        printf("    %ld", tpool->thpool_arr[loopv]->threadid);
    }
    printf("   ]\n");
    printf("\tJob Queue: %d size\n", tpool->job_queue->size);
    printf("\tWork Function: %p\n", tpool->work_f);
    printf("\tPool size: %d\n", tpool->size);
    printf("\tWorking threads: %d\n", tpool->working);
    return 0;
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
        sleep(1);
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

    pthread_mutex_lock(&(threads_p)->pool->poolmutex);
    threads_p->pool->alive += 1; //increment the alive threads
    pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

    while (THPOOL_ALIVE) {

        condwait(threads_p->pool->job_queue->qlock);

        pthread_mutex_lock(&(threads_p)->pool->poolmutex);
        (threads_p)->pool->working += 1;
        pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

        request_t *req = Dequeue((threads_p)->pool->job_queue);
        void* ret;
        if(req == NULL)
            condwait(threads_p->pool->job_queue->qlock);
        else{
            ret = threads_p->pool->work_f->f1(req);
            if(threads_p->pool->work_f->f2 != NULL && ret != NULL)
                threads_p->pool->work_f->f2(ret);
        }

        pthread_mutex_lock(&(threads_p)->pool->poolmutex);
        (threads_p)->pool->working -= 1;
        pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

    }
    pthread_mutex_lock(&(threads_p)->pool->poolmutex);
    (threads_p)->pool->alive -= 1;
    pthread_mutex_unlock(&(threads_p)->pool->poolmutex);

}

void cust_work(cust* cust_args) {
    struct sigaction inter; /*Interrupt thread activity and put the thread on hold*/
    sigemptyset(&inter.sa_mask);
    inter.sa_handler = hold;

    pthread_mutex_lock(&cust_args->thread->pool->poolmutex);
    cust_args->thread->pool->alive += 1; //increment the alive threads
    pthread_mutex_unlock(&cust_args->thread->pool->poolmutex);

    while (THPOOL_ALIVE) {
        pthread_cond_wait(cust_args->cond, NULL);

        pthread_mutex_lock(&cust_args->thread->pool->poolmutex);
        cust_args->thread->pool->working += 1;
        pthread_mutex_unlock(&cust_args->thread->pool->poolmutex);

        //cust_args->f1();
    }


}

int cust_thread(callback f, callback f2, pthread_cond_t* cond, thpool_t* pool) {
    pthread_mutex_lock(&pool->poolmutex);

    pool->size += 1;
    pool->thpool_arr[pool->size] = (threads *)malloc(sizeof(threads));
    if(pool->thpool_arr[pool->size]==NULL)
        return -1;
    pool->thpool_arr[pool->size]->pool = pool;
    pool->thpool_arr[pool->size]->id = pool->size;

    pthread_mutex_unlock(&pool->poolmutex);

    cust* cust_args = (cust*)malloc(sizeof(cust));
    if(cust_args == NULL)
        return -1;
    cust_args->thread = pool->thpool_arr[pool->size];
    cust_args->cond = cond;
    cust_args->f1 = f;
    cust_args->f2 = f2;

    pthread_create(&pool->thpool_arr[pool->size]->threadid, NULL, (void *)cust_work, (void *)cust_args);
    pthread_detach(pool->thpool_arr[pool->size]->threadid);

    return 0;
}





void thkill(threads* thread_p) {

    free(thread_p);

}