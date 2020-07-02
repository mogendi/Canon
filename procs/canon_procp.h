//
// Created by nelson on 1/27/20.
//

#ifndef CANON_CANON_PROCP_H
#define CANON_CANON_PROCP_H

#include "canon_tpool.h"
#include "../http/handler.h"
#include "../core/connection.h"
#include <unistd.h>

/*
 *    p2
 *   //
 * pool == p1
 *   \\
 *    p3
 *
 * The pool(main proc) tracks all forked() procs and grows/shrinks the pool
 * as necessary. It sends instructions over pipes (die) and reads idle times
 * from it.
 * */

#define RD    0
#define WR    1
#define DIE   2
#define WAIT  3

typedef struct proc proc_t;
typedef struct procs_p procs_t;


/*effectively the main process*/
struct procs_p {
    thpool_t* pool;

    proc_t* or;

    int idle; //avg idle time for all proc
    sock* ch; //listen socket
};

struct proc {
    pid_t pid;

    int l_fd;
    int pipe[2][2];

    int idl;

    unsigned or: 1;

    pthread_mutex_t l; // locked when handling
    pthread_cond_t c; //wake up extra thread when done handling
    pthread_t id; //read/write thread

    unsigned dead:1;
    procs_t* pool;
};

int start_up();

#endif //CANON_CANON_PROCP_H
