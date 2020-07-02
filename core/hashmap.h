#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

/*
 * TODO: ht_to_text()
 * */

typedef struct entry_s entry_t;

typedef struct hashtable_s hashtable_t;

// Create a new hashtable.
hashtable_t *ht_create( int size );

// Insert a key-value pair into a hash table.
void ht_set( hashtable_t *hashtable, char *key, char *value );

// Retrieve a key-value pair from a hash table.
char *ht_get( hashtable_t *hashtable, char *key );

void ht_destroy(hashtable_t *hashmap);

int ht_compare(hashtable_t *hashmapl, hashtable_t *hashmapr);

#endif // HASHMAP_H_INCLUDED
