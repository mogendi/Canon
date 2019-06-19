//
// Created by nelson on 13/05/2019.
//

#ifndef CANON_BST_H
#define CANON_BST_H

#include "request.h"

/*
 * Standard BST API where protocols can use the structure
 * to extend their logic.
 * TODO: Make the BST more like AVL trees
 * */


typedef void (*callback)(void* data);

typedef struct node_p node;

/*The BST structure
 * In Interest of maintaining a 'light'
 * implementation the bst structure will only have
 * necessary references to make it "memory light"
 * */

typedef struct bst_p bst;


/*                  BST API
 * =======================================*/

void insert(bst* bst_l, request_t* Req);

node* search(request_t* Req, node* start);

int delete(request_t* Req, bst* bst_l);


#endif //CANON_BST_H
