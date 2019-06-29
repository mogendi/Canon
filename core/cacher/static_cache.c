//
// Created by nelson on 6/22/19.
//

#include "mutex.h"
#include "../DataStructures/request.h"
#include "request_cacher.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

/*
 * Explicitly Caches Requests
 * */

/*The stored types are requests therefore
 * a method of deriving integers from
 * their message is necessary
 * */
static int dry_hash( int size, request_t* bst_l) {

    unsigned long int hashval;
    int i = 0;

    // Convert our string to an integer
    while( hashval < ULONG_MAX && i < strlen( bst_l->MSG ) && bst_l->MSG[i] != "\r") {
        hashval = hashval << 8;
        hashval += bst_l->MSG[ i ];
        i++;
    }

    return hashval % size;
}


static int find_val(  );

/*
 *                 CACHE API IMPL
 * -------------------------------------------------*/

cache_data* cache_init() {
    cache_data* bst_l = create_bst();
    if(bst_l == NULL)
        return NULL;

    
}