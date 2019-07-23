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
 * TODO: Make the cache_init function take a
 *       static path file dir from config
 * */


/*
 *                  CACHE NODE FUNCTIONS
 * -------------------------------------------------*/

static fim_t* create_cache_node(char* file_name,
                                time_t expiry, cache_t* parent) {

    fim_t* entry = (fim_t *)malloc(sizeof(struct cache_node));
    if(entry == NULL)
        return NULL;

    entry->f_headers = *new_file_header(file_name);
    entry->creat_time = time(NULL);
    entry->expiry = expiry;
    entry->hits = 0;
    entry->parent = parent;
    entry->f_data = NULL;
    entry->valid = 0;
    pthread_mutex_init(&entry->cn_lock, NULL);
    retun entry
}

static cache_t* create_cache(cache_data* data, char* dir) {
    cache_t* cache_s = (cache_t*)malloc(sizeof(cache_t));
    cache_s->data = data;
    cache_s->dir = new_dir(dir);
    cache_s->size = cache_s->dir->size;
}

/*
 *                 CACHE API IMPL
 * -------------------------------------------------*/

cache_t* cache_init(char* path) {
    cache_data* bst_l = create_bst();
    cache_t* cache_s = create_cache(bst_l, path);
    if(bst_l == NULL || cache_s == NULL)
        return NULL;

    int loopv = 0;
    while(loopv < cache_s->size) {

    }
}
