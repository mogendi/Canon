//
// Created by nelson on 6/16/19.
//

#include "bst.h"
#include "mutex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>



/*                        TYPE IMPLEMENTATIONS & PROTOTYPES
------------------------------------------------------------------------------------*/
struct bst_p{
    node* Head;
    int size;
};

struct node_p{
    int val;
    request_t* request;
    node* left;
    node* right;
};

void pre_order(node* root);
node* find_min(node* root);
node* find_max(node* root);
void breadth_first(node* root, callback f);
node* find_parent(request_t* Req, node* start);
int check_balance(node* root);
/*----------------------------------------------------------------------------------*/



/*The insert method needs a recursive
 *method of placing the nodes
 *in the correct position
*/
void place_value(node* root, node* in) {
    if(root->val > in->val) {
        if(root->left == NULL)
            root->left = in;
        else
            place_value(root->left, in);
    }
    else if(root->val < in->val) {
        if(root->right == NULL)
            root->right = in;
        else
            place_value(root->right, in);
    } else {
        /*kill the node*/
    }
    if(check_balance(root) == 1) {
        printf("BST Edited, re-balanced");
        fflush(stdout);
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

void breadth_first(node* root, callback f) {
    if(root == NULL)
        return;
    /*Unimplemented*/
}

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

void right_rotate(node* root) {
    node* left_node = root->left;
    root->left = left_node->right;
    left_node->right = root;
    node* parent = find_parent(root->request, root);
    if(root->val > parent->val)
        parent->right = left_node;
    else
        parent->left = left_node;
}

void left_rotate(node* root) {
    node* right_node = root->right;
    root->right = right_node->left;
    right_node->left = root;
    node* parent = find_parent(root->request, root);
    if(root->val > parent->val)
        parent->right = right_node;
    else
        parent->left = right_node;
}

void left_right(node* root) {
    left_rotate(root->left);
    right_rotate(root);
}

void right_left(node* root) {
    right_rotate(root->right);
    left_rotate(root);
}

int height(node* root, int h) {
    int left_h = h, right_h = h;
    if(root->left != NULL || root->right != NULL)
        h += 1;
    if(root->left != NULL) {
        left_h = height(root->left, h);
    }
    if(root->right != NULL) {
        right_h = height(root->right, h);
    }
    if(left_h > right_h) {
        return left_h;
    } else { return right_h; }
}

/*Return 0 if no re-balancing happened otherwise returns 1*/
int check_balance(node* root) {
    int flag = 0;

    if((height(root->left, 0)- height(root->right, 0))>1) {
        if((height(root->left->left, 0)- height(root->left->right, 0))>0) {
            right_rotate(root);
            flag = 1;
        } else {
            left_right(root);
            flag = 1;
        }
    }

    if((height(root->left, 0)- height(root->right, 0))<-1) {
        if((height(root->right->left, 0)- height(root->right->right, 0))<0) {
            left_rotate(root);
            flag = 1;
        } else {
            right_left(root);
            flag = 1;
        }
    }

    return flag;

}


/*       API IMPLEMENTATION
 *-------------------------------*/

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
