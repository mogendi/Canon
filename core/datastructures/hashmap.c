//
// Created by nelson on 6/9/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "hashmap.h"


/*          TYPE IMPLEMENTATIONS & PROTOTYPES
 * -------------------------------------------------*/

struct entry_s {
    char *key;
    char *value;
    struct entry_s *next;
};

struct hashtable_s {
    int size;
    struct entry_s **table;
};

// Hash a string for a particular hash table.
int ht_hash( hashtable_t *hashtable, char *key );

// Create a key-value pair.
entry_t *ht_newpair( char *key, char *value );



// Create a new hashtable.
hashtable_t *ht_create( int size ) {

    hashtable_t *hashtable = NULL;
    int i;

    if( size < 1 ) return NULL;

    // Allocate the table itself.
    if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
        return NULL;
    }

    // Allocate pointers to the head nodes.
    if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
        return NULL;
    }
    for( i = 0; i < size; i++ ) {
        hashtable->table[i] == NULL;
    }

    hashtable->size = size;

    return hashtable;
}


// Hash a string for a particular hash table.
int ht_hash( hashtable_t *hashtable, char *key ) {

    unsigned long int hashval;
    int i = 0;

    // Convert our string to an integer
    while( hashval < ULONG_MAX && i < strlen( key ) ) {
        hashval = hashval << 8;
        hashval += key[ i ];
        i++;
    }

    return hashval % hashtable->size;
}

// Create a key-value pair.
entry_t *ht_newpair( char *key, char *value ) {
    entry_t *newpair;

    if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
        return NULL;
    }

    if( ( newpair->key = strdup( key ) ) == NULL ) {
        return NULL;
    }

    if( ( newpair->value = strdup( value ) ) == NULL ) {
        return NULL;
    }

    newpair->next = NULL;

    return newpair;
}


void ht_set( hashtable_t *hashtable, char *key, char *value ) {
    int bin = 0;
    entry_t *newpair = NULL;
    entry_t *next = NULL;
    entry_t *last = NULL;

    bin = ht_hash( hashtable, key );

    next = hashtable->table[ bin ];

    while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
        last = next;
        next = next->next;
    }

    // There's already a pair.  Let's replace that string.
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {

        free( next->value );
        next->value = strdup( value );

        // Nope, could't find it.  Time to grow a pair.
    } else {
        newpair = ht_newpair( key, value );

        // We're at the start of the linked list in this bin.
        if( next == hashtable->table[ bin ] ) {
            newpair->next = next;
            hashtable->table[ bin ] = newpair;

            // We're at the end of the linked list in this bin.
        } else if ( next == NULL ) {
            last->next = newpair;

            // We're in the middle of the list.
        } else  {
            newpair->next = next;
            last->next = newpair;
        }
    }
}

// Retrieve a key-value pair from a hash table.
char *ht_get( hashtable_t *hashtable, char *key ) {
    int bin = 0;
    entry_t *pair;

    bin = ht_hash( hashtable, key );

    // Step through the bin, looking for our value.
    pair = hashtable->table[ bin ];
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
        pair = pair->next;
    }

    // Did we actually find anything?
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        return NULL;
    } else {
        return pair->value;
    }

}

void ht_destroy(hashtable_t *hashmap){
    int bin = 0;
    entry_t *pair;

    while( bin != hashmap->size){
        pair = hashmap->table[bin];
        if( pair == NULL || pair->key == NULL ){
            /**/
        } else {
            free(pair);
        }
    }
    free(hashmap->table);
    free(hashmap);
}

int ht_compare(hashtable_t *hashmapl, hashtable_t *hashmapr){
    int loopv, sl_value;
    if(hashmapl == NULL && hashmapr == NULL) {
        printf("Both hashmaps are empty");
        return 0;
    }

    if(hashmapl == NULL || hashmapr == NULL) {
        printf("One of the hashmaps is empty");
        return 1;
    }

    if(hashmapl->size < hashmapr->size)
        sl_value = hashmapl->size;
    else{sl_value = hashmapr->size;}

    for(loopv = 0; loopv<sl_value; loopv++ ) {
        if(strcmp(hashmapl->table[loopv]->value, hashmapr->table[loopv]->value) !=0 ){
            return 1;
        }
    }

    return 0;

}