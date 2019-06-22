#ifndef CANON_TPOOL_H_INCLUDED
#define CANON_TPOOL_H_INCLUDED

#include "../DataStructures/queue.h"


/*
 *    Job Que
 *  [---------] ________________
 *                              |
 *                              |
 *                             \|/
 *                  ------------------------------------
 *                 | Functions that control threadpool  |
 *                 | access and assignment of tasks     |
 *                  ------------------------------------
 *
 *                  [T1 T2 T3.....Tn] Thread pool
 *
 *  The worker pool is active (tries to fetch requests from the queue) therefore
 *  a mutex lock on the entire queue is necessary.
 *  Essentially this is a producer consumer problem, the queue therefore uses
 *  a monitor to control concurrent access to its types
 *
 *
 * */

// Monitor for threads trying to access the queue

typedef struct thpool thpool_t;
typedef struct threads_p threads;
typedef struct threads_cust cust;
typedef void* (*callback)(void* data);
typedef struct funcs_t funcs; //Work functions

struct thpool{
    threads **thpool_arr; //Pointer to the array of threads
    queue* job_queue; //Pointer to the queue (Defined in core/Data structures/queue.h)
    funcs* work_f;
    int size;
    int alive; //Number of alive threads
    int working; // Number of working threads
    pthread_mutex_t poolmutex; //Mutex for the thread pools types
    pthread_cond_t cond; // Conditional variable to control pool activity
};

struct threads_cust{
    threads* thread;
    callback f1;
    callback f2;
    pthread_cond_t* cond;
};

struct funcs_t {
    callback f1;
    callback f2;
};

//Return an initialized thpool array
thpool_t* pool_init(callback f1, callback f2);

//Destroys the entire pool
void pool_kill(thpool_t* tpool);

/*custom thread with specified behaviour
 * not tied to job queues*/
int cust_thread(callback f, callback f2, pthread_cond_t* cond, thpool_t* pool);

//Polls the threads for its information
int poll(thpool_t* tpool);

//Pauses threads, un-pauses if they're all paused
void pause_pool(thpool_t* tpool);

//Resumes threads, pauses them if active
void resume_pool(thpool_t* tpool);


#endif // CANON_TPOOL_H_INCLUDED
