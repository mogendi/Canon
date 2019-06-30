//
// Created by nelson on 6/22/19.
//

#include "mutex.h"
#include "static_cache.h"
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "datastructures/bst.h"


/*
 *                  CACHE NODE FUNCTIONS
 * -------------------------------------------------*/

static fim_t* create_cache_node(char* file_name,
                                time_t expiry, cache_t* parent) {

    fim_t* entry = (fim_t *)malloc(sizeof(struct cache_node));
    if(entry == NULL)
        return NULL;

    entry->f_headers = *create_file_header(file_name);
    entry->creat_time = time(NULL);
    entry->expiry = expiry;
    entry->hits = 0;
    entry->parent = parent;
    entry->f_data = NULL;
    entry->valid = 0;
    pthread_mutex_init(&entry->cn_lock, NULL);

}


/*
 *                 CACHE API IMPL
 * -------------------------------------------------*/

cache_data* cache_init() {
    cache_data* bst_l = create_bst();
    if(bst_l == NULL)
        return NULL;
}