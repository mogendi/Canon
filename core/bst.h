//
// Created by nelson on 13/05/2019.
//

#ifndef CANON_BST_H
#define CANON_BST_H
#include <time.h>

/*
 * Standard BST API where protocols can use the structure
 * to extend their logic.
 * */

/*
 * ISSUES: the tight coupling of the BSTs and
 *         request types
 * */


typedef void (*callback)(void* data);

typedef struct node node_t;

/*The BST structure
 * In Interest of maintaining a 'light'
 * implementation the bst structure will only have
 * necessary references to make it "memory light"
 * */

typedef struct bst_p bst;

struct bst_p{
    node_t* Head;
    int size;
};

struct node{
    int val;
    void* request;
    node_t* left;
    node_t* right;
    bst* bst_l; //Head of the parent BST
    int freq;
    time_t access;
};

/*                  BST API
 * =======================================*/


/* The BST logically doesn't need to keep all of it's
 * members in a contagious memory location therefore a size
 * doesn't have to be defined up front*/
bst* create_bst();

/*
 * The insert function will allow you to
 * pass a void pointer for any data you might
 * want to store that isn't the actual node value
 * */
void insert(int value, bst* bst_l, void* Req);

node_t* search(int value, node_t* start);

int delete(int value, bst* bst_l);

int destroy_bst(bst* bst_l);

#endif //CANON_BST_H
