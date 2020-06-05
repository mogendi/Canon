//
// Created by nelson on 1/27/20.
//

#ifndef CANON_CANON_PROCP_H
#define CANON_CANON_PROCP_H

#include "canon_tpool.h"
#include <unistd.h>
#include "canon_tpool.h"

typedef struct proc proc_t;
typedef struct procp procp_t;

struct procp { //effectively the main process
    proc_t **proc_arr; //Limit is to be set by config
    queue* reqs;
    thpool_t* pool;
    /*connection info + anything else*/
};

struct proc {
    queue* reqs;
    thpool_t* pool;

};

#endif //CANON_CANON_PROCP_H
