//
// Created by user on 13/05/2019.
//

#ifndef CANON_BST_H
#define CANON_BST_H

#include "request.h"
#include "mutex.h"

/*
 * Standard BST API where protocols can use the structure
 * to extend their logic.
 * TODO: Make the BST more like AVL trees
 * */

typedef struct node_p node;

struct node_p{
    int val;
    request_t* request;
    node* left;
    node* right;
};

/*The BST structure
 * In Interest of maintaining a 'light'
 * implementation the bst structure will only have
 * necessary references to make it "memory light"
 * */

typedef struct bst_p bst;

struct bst_p{
    node* Head;
    int size;
};


/*The insert method needs a recursive
 *method of placing the nodes
 *in the correct position
*/
void place_value(node* root, node* in) {
    if(root->val < in->val) {
        if(root->left == NULL)
            root->left = in;
        else
            place_value(root->left, in);
    }
    else if(root->val > in->val) {
        if(root->right == NULL)
            root->right = in;
        else
            place_value(root->right, in);
    } else {
        /*kill the node*/
    }
}

/*The stored types are requests therefore
 * a method of deriving integers from
 * their message is necessary
 * */
void dry_hash( int size, node* bst_l) {

    unsigned long int hashval;
    int i = 0;

    // Convert our string to an integer
    while( hashval < ULONG_MAX && i < strlen( bst_l->request->MSG ) ) {
        hashval = hashval << 8;
        hashval += bst_l->request->MSG[ i ];
        i++;
    }

    bst_l->val = hashval % size;
}


/*               NODE FUNCS
 * ------------------------------------*/

node* create_node(request_t* Req) {
    node* new_n = (node *)malloc(sizeof(node));
    if(new_n == NULL){
        printf("Failed to initialize BST");
        exit(1);
    }

    new_n->request = Req;
    new_n->left = NULL;
    new_n->right = NULL;
    dry_hash(Req->sockfd,new_n);
}


/*              BST FUNCS
 * ------------------------------------*/

node* find_parent(request_t* Req, node* start) {
    node* search_node = create_node(Req);

    if(start == NULL)
        return NULL;

    if(search_node->val == start->val)
        return NULL;

    if(search_node->val < start->val) {
        if(start->left == NULL)
            return NULL;
        else if(start->left->val == search_node->val)
            return start;
        else
            find_parent(Req, start->left);
    } else {
        if(start->right == NULL)
            return NULL;
        else if(start->right->val == search_node->val)
            return start;
        else
            find_parent(Req, start->right);
    }
}

void pre_order(node* root) {
    if(root == NULL)
        return;
    pre_order(root->left);
    pre_order(root->right);
}

node* find_min(node* root) {
    if(root->left == NULL)
        return root;
    find_min(root->left);
}

node* find_max(node* root) {
    if(root->right == NULL)
        return root;
    find_max(root->right);
}

void breadth_first(node* root) {
    if(root == NULL)
        return;
    /*Unimplemented*/
}

/*                  BST API
 * =======================================*/



void insert(bst* bst_l, request_t* Req) {
    node* in = create_node(Req);
    if(bst_l->Head == NULL) {
        bst_l->Head = in;
     } else {
        place_value(bst_l->Head,in);
    }
}

node* search(request_t* Req, node* start) {
    node* search_node = create_node(Req);
    int val = search_node->val;

    if(start == NULL)
        return NULL;

    if(start->val == val)
        return start;
    else if(start->val < val)
        search(Req, start->left );
    else
        search(Req, start->right);
}

int delete(request_t* Req, bst* bst_l) {
    if(bst_l == NULL)
        return 1;

    node* del = search(Req, bst_l->Head);
    node* parent = find_parent(Req, bst_l->Head);

    if(del == NULL)
        return 1;

    if(bst_l->size == 1) {
        bst_l->size = 0;
        free(bst_l->Head);
        bst_l->Head = NULL;
        return 0;
    }

    if(del->left == NULL && del->right == NULL) {
        //If the node is a leaf node
        if(del->val < parent->val)
            parent->left = NULL;
        if(del->val > parent->val)
            parent->right = NULL;
        bst_l->size -= 1;
        free(del);
        return 0;
    }

    if(del->left != NULL && del->right == NULL) {
        //If the node has a left subtree ONLY
        if(del->val < parent->val)
            parent->left = del->left;
        if(del->val > parent->val)
            parent->right = del->left;
        bst_l->size -= 1;
        free(del);
        return 0;
    }

    if(del->left == NULL && del->right != NULL) {
        //If the node has a right subtree ONLY
        if(del->val < parent->val)
            parent->left = del->right;
        if(del->val > parent->val)
            parent->right = del->right;
        bst_l->size -= 1;
        free(del);
        return 0;
    }

    if(del->left == NULL && del->right == NULL) {
        //If the node has a left and right subtree
        node* largest = find_max(del->left);
        node* parent_l = find_parent(largest->request, del->left);
        parent_l->right = NULL;
        if(del->val < parent->val)
            parent->left = largest;
        if(del->val > parent->val)
            parent->right = largest;
        bst_l->size -= 1;
        free(del);
        return 0;
    }

}


#endif //CANON_BST_H
