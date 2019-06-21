//
// Created by nelson on 13/05/2019.
//

#ifndef CANON_BST_H
#define CANON_BST_H

#include "request.h"

/*
 * Standard BST API where protocols can use the structure
 * to extend their logic.
 * */

/*
 * ISSUES: the tight coupling of the BSTs and
 *         request types
 * TODO: Make the BSTs API more Abstract
 * */


typedef void (*callback)(void* data);

typedef struct node node_t;

/*The BST structure
 * In Interest of maintaining a 'light'
 * implementation the bst structure will only have
 * necessary references to make it "memory light"
 * */

typedef struct bst_p bst;


/*                  BST API
 * =======================================*/


/* The BST logically doesn't need to keep all of it's
 * members in a contagious memory location therefore a size
 * doesn't have to be defined up front*/
bst* create_bst();

void insert(bst* bst_l, request_t* Req);

node_t* search(request_t* Req, node_t* start);

int delete(request_t* Req, bst* bst_l);

#endif //CANON_BST_H
