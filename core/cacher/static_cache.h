//
// Created by nelson on 6/21/19.
//

#ifndef CANON_REQUEST_CACHER_H
#define CANON_REQUEST_CACHER_H

#endif //CANON_REQUEST_CACHER_H

#include <zconf.h>
#include "datastructures/bst.h"
#include "./can_file.h"

#define HIT 0
#define MIS 1
#define LIM 1000 /*Cache size limit*/

/* Extends BSTs logic to cache static resources.
 * Is an implementation of "weighted"
 * LFU algorithm, that takes into consideration
 * both time of last request access and how often
 * the static resource is searched for.
 * */

typedef struct bst_p cache_data;

typedef struct cache_node fim_t;

typedef struct cache cache_t;

struct cache {
    cache_data* data;

    dir_t* dir;

    int size;
};

struct cache_node {
    file_t f_headers;
    char* f_data;

    cache_t* parent;

    int hits;

    unsigned valid:1;
    unsigned dirty:1;

   time_t expiry;
   time_t creat_time;
   time_t update;

   pthread_mutex_t cn_lock;
};


/*
 * On start-up, all the files in the static
 * file directory are given a cache entry
 * */
cache_t* cache_init(char* path);



int cache_destroy();


fim_t* cache_search( char* fname, cache_t* cache);

int cache_remove(cache_t* cache);


int force_update(cache_data* cache);

