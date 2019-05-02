#ifndef CANON_TPOOL_H_INCLUDED
#define CANON_TPOOL_H_INCLUDED

#include "request.h"
#include "queue.h"
#include <pthread.h>
#include <signal.h>
#include "Parser.h"

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

//Thread types
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

//Return an initialized thpool array
thpool_t* pool_init(int size, queue* jobQ);

//Creates threads ie; populates the thread pool
static int thinit(thpool_t* pool, threads** thread_p, int id);

//Assigns a request to a thread from a queue "Dispatcher"
int work(threads** threadp);
/*
 * The threads are active, they'll dequeue the work pool
 * and call parse(HTTP MSG PARSE) on them
 * TODO Make thread "work" function agnostic
 * */

//Destroys the entire pool
void destroy_pool(thpool_t* tpool);

//Polls for any working threads, return int array of all non working arrays
int* poll(thpool_t* tpool);

//Pauses threads, un-pauses if they're all paused
void pause_pool(thpool_t* tpool);

//Resumes threads, pauses them if active
void resume_pool(thpool_t* tpool);


#endif // CANON_TPOOL_H_INCLUDED
