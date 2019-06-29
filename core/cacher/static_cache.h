//
// Created by nelson on 6/21/19.
//

#ifndef CANON_REQUEST_CACHER_H
#define CANON_REQUEST_CACHER_H

#endif //CANON_REQUEST_CACHER_H

#include "DataStructures/request.h"
#include "DataStructures/bst.h"

#define HIT 0
#define MIS 1
#define LIM 1000 /*Cache size limit*/

/* Extends BSTs logic to cache requests.
 * Is an implementation of "weighted"
 * LFU algorithm, that takes into consideration
 * both time of last request access and how often
 * the request is searched for.
 * */

typedef struct bst_p cache_data;

cache_data* cache_init();

/*
 * Will return the requests response if it found
 * the request in the cache data, return NULL otherwise
 * */
resp_t* cache_search( request_t* Req, cache_data* cache );

/*
 * Force the cache to update it's data tree
 * and clean the Oldest and lest frequently used
 * value
 * */
int force_update(cache_data* cache);

