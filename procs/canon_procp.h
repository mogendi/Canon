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
 * as necessary. It rcvs idle times over signals.
 * */

typedef struct proc proc_t;
typedef struct procs_p procs_t;


/*effectively the main process*/
struct procs_p {
    proc_t* or;

    queue* q;

    int idl; //avg idle time for all proc
    int wrk_ttl;

    sock* ch; //listen socket

    pid_t mpid;
};

struct proc {
    pid_t pid;

    int l_fd;

    int idl;

    unsigned or: 1;

    unsigned dead:1;
    procs_t* pool;
};

int start_up();

#endif //CANON_CANON_PROCP_H
