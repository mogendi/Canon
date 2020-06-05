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
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "bst.h"

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
    entry->update= time(NULL);
    entry->expiry = expiry;
    entry->hits = 0;
    entry->parent = parent;
    entry->f_data = NULL;
    entry->valid = 0;
    pthread_mutex_init(&entry->cn_lock, NULL);
    return entry;
}

static cache_t* create_cache(cache_data* data, char* dir) {
    cache_t* cache_s = (cache_t*)malloc(sizeof(cache_t));
    cache_s->data = data;
    cache_s->dir = new_dir(dir);
    cache_s->size = cache_s->dir->size;
}

/*Returns 0 if invalid 1 otherwise*/
static int check_cache_valid(fim_t* cache_node) {
    if(cache_node->f_data == NULL)
        return 0;
    time_t expiry = cache_node->update + cache_node->expiry;
    if(time(NULL) > expiry)
        return 0;
    else
        return 1;
}

static int swap_data(fim_t* cache_node) {
    munmap(cache_node->f_data, cache_node->f_headers.info.st_size);
    stat(cache_node->f_headers.path, &cache_node->f_headers.info);
    cache_node->f_headers.fd = open(cache_node->f_headers.path, O_RDWR, S_IRUSR | S_IWUSR);
    cache_node->f_data = mmap(NULL, cache_node->f_headers.info.st_size, PROT_READ, MAP_PRIVATE, cache_node->f_headers.fd, 0);
    close(cache_node->f_headers.fd);
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
    while(loopv < cache_s->dir->data->size) {
        fim_t* cursor = create_cache_node(cache_s->dir->data->data[loopv], 2000, cache_s);
        insert(cursor->f_headers.fname_hash, bst_l, cursor);
        loopv++;
    }

    return cache_s;
}


fim_t* cache_search(char* fname, cache_t* cache) {
    uint32_t fn_hash = hash(fname);
    fim_t* cache_node = search(fn_hash, cache->data->Head)->request;

    if(cache_node == NULL) {
        return NULL;
    }

    cache_node->hits++;

    if(check_cache_valid(cache_node)) {
        return cache_node;
    } else {
        if(cache_node->hits >= HITS_MAX) {
            uint32_t f_crc = crc(&cache_node->f_headers);

            if(f_crc != cache_node->f_headers.crc32) {
                cache_node->f_headers.crc32 = f_crc;
                swap_data(cache_node);
                return cache_node;
            }

            cache_node->f_headers.fd = open(cache_node->f_headers.path, O_RDWR, S_IRUSR | S_IWUSR);
            stat(cache_node->f_headers.path, &cache_node->f_headers.info);
            cache_node->f_data = mmap(NULL, cache_node->f_headers.info.st_size, PROT_READ, MAP_PRIVATE, cache_node->f_headers.fd, 0);
            cache_node->valid = 1;
        } else {
            printf("Cache node is invalid\n");
            return NULL;
        }
    }

    return cache_node;
}

/*Maintains the state of everything in the cache graph*/
fim_t* cache_search_stable(char* fname, cache_t* cache) {
    uint32_t fn_hash = hash(fname);
    fim_t* cache_node = search(fn_hash, cache->data->Head)->request;
    return cache_node;
}

int cache_remove(fim_t* cache_node) {
    
}