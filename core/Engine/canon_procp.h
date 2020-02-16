//
// Created by nelson on 1/27/20.
//

#ifndef CANON_CANON_PROCP_H
#define CANON_CANON_PROCP_H

#include "canon_tpool.h"
#include <unistd.h>

typedef struct proc proc_t;
typedef struct procp procp_t;

struct procp {
    proc_t **proc_arr; //Limit is to be set by config
    queue* reqs;

};

#endif //CANON_CANON_PROCP_H
