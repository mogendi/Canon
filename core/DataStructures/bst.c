//
// Created by nelson on 6/16/19.
//

#include "bst.h"
#include "mutex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>



/*                        TYPE IMPLEMENTATIONS & PROTOTYPES
------------------------------------------------------------------------------------*/

struct node{
    int val;
    void* request;
    node_t* left;
    node_t* right;
    bst* bst_l; //Head of thr parent BST
    int freq;
    time_t access;
};

int check_balance(node_t* root);
void place_value(node_t* root, node_t* in);
/*----------------------------------------------------------------------------------*/



/*               NODE FUNCS
 * ------------------------------------*/

node_t* create_node(int value, void* Req) {
    node_t* new_n = (node_t *)malloc(sizeof(node_t));
    if(new_n == NULL){
        printf("Failed to initialize BST");
        exit(1);
    }

    new_n->request = Req;
    new_n->left = NULL;
    new_n->right = NULL;
    new_n->freq = 0;
    new_n->val = value;
    new_n->access = time(NULL)%3600;
    return new_n;
}


/*              BST FUNCS
 * ------------------------------------*/

void pre_order(node_t* root, callback f) {
    if(root == NULL)
        return;
    f(root);
    pre_order(root->left, f);
    pre_order(root->right, f);
}

void post_order(node_t* root, callback f) {
    if(root == NULL)
        return;
    post_order(root->left, f);
    post_order(root->right, f);
    f(root);
}

node_t* find_min(node_t* root) {
    if(root->left == NULL)
        return root;
    find_min(root->left);

    return NULL;
}

node_t* find_max(node_t* root) {
    if(root->right == NULL)
        return root;
    find_max(root->right);

    return NULL;
}

node_t* find_parent(int value, node_t* start) {
    int val = value;
    if(start == NULL)
        return NULL;

    if(val == start->val)
        return NULL;

    if(val < start->val) {
        if(start->left == NULL)
            return NULL;
        else if(start->left->val == val)
            return start;
        else
            return find_parent(value, start->left);
    } else if(val > start->val){
        if(start->right == NULL)
            return NULL;
        else if(start->right->val == val)
            return start;
        else
            return find_parent(value, start->right);
    } else {  }

    return NULL;
}

void right_rotate(node_t* root) {
    if(root == NULL || root->left == NULL)
        return;
    node_t* parent = find_parent(root->val, root->bst_l->Head);
    node_t* left_node = root->left;
    root->left = left_node->right;
    left_node->right = root;
    if(parent == NULL) {
        root->bst_l->Head = left_node;
        return;
    }
    if(root->val > parent->val)
        parent->right = left_node;
    else
        parent->left = left_node;
}

void left_rotate(node_t* root) {
    if(root == NULL || root->right == NULL)
        return;
    node_t* parent = find_parent(root->val, root->bst_l->Head);
    node_t* right_node = root->right;
    root->right = right_node->left;
    right_node->left = root;
    if(parent == NULL) {
        root->bst_l->Head = right_node;
        return;
    }
    if(root->val > parent->val)
        parent->right = right_node;
    else
        parent->left = right_node;
}

void left_right(node_t* root) {
    left_rotate(root->left);
    right_rotate(root);
}

void right_left(node_t* root) {
    right_rotate(root->right);
    left_rotate(root);
}

int height(node_t* root, int h) {
    if(root == NULL)
        return 0;

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
        return left_h+1;
    } else { return right_h+1; }
}

/*Return 0 if no re-balancing happened otherwise returns 1*/
int check_balance(node_t* root) {
    if(root == NULL)
        return 1;

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

/*The insert method needs a recursive
 *method of placing the nodes
 *in the correct position
*/
void place_value(node_t* root, node_t* in) {
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
    if(check_balance(root) == 1) { }
    if(check_balance(root->left) == 1) { }
    if(check_balance(root->right) == 1) { }

}

/*       API IMPLEMENTATION
 *-------------------------------*/

bst* create_bst() {
    bst* bst_l = (bst *)malloc(sizeof(bst));

    if(bst_l == NULL) {
        printf("Memory allocation for bst failed");
        fflush(stdout);
        return NULL;
    }

    bst_l->Head = NULL;
    bst_l->size = 0;
    return bst_l;
}

void insert(int value, bst* bst_l, void* Req) {
    node_t* in = create_node(value, Req);
    in->bst_l = bst_l;
    if(bst_l->Head == NULL) {
        bst_l->Head = in;
    } else {
        place_value(bst_l->Head,in);
    }
    bst_l->size += 1;
}

node_t* search(int value, node_t* start) {
    int val = value;

    if(start == NULL)
        return NULL;

    if(start->val == val) {
        start->freq += 1;
        start->access = time(NULL);
        start->access = start->access%3600;
        return start;
    }

    else if(val < start->val)
        search(val, start->left );
    else
        search(val, start->right);

    return NULL;
}

int delete(int value, bst* bst_l) {
    if(bst_l == NULL)
        return 1;

    node_t* del = search(value, bst_l->Head);
    node_t* parent = find_parent(value, bst_l->Head);

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

    if(del->left != NULL && del->right != NULL) {
        //If the node has a left and right subtree
        node_t* largest = find_max(del->left);
        node_t* parent_l = find_parent(largest->val, del->left);
        parent_l->right = NULL;
        if(del->val < parent->val)
            parent->left = largest;
        if(del->val > parent->val)
            parent->right = largest;
        bst_l->size -= 1;
        free(del);
        return 0;
    }
    node_t* l2 = find_parent(parent->val, bst_l->Head);
    if(l2 != NULL) {
        int trace = height(l2, 0);
        while (trace > 0) {
            check_balance(l2);
            check_balance(l2->left);
            check_balance(l2->right);

            trace--;
        }
    }

    return 1;

}
