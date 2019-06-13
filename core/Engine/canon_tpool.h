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


//Return an initialized thpool array
thpool_t* pool_init(int size, queue* jobQ);

//Destroys the entire pool
void pool_kill(thpool_t* tpool);

//Polls for any working threads, return int array of all non working threads
int* poll(thpool_t* tpool);

//Pauses threads, un-pauses if they're all paused
void pause_pool(thpool_t* tpool);

//Resumes threads, pauses them if active
void resume_pool(thpool_t* tpool);


#endif // CANON_TPOOL_H_INCLUDED
