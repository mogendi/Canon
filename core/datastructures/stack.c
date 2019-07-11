#include "stack.h"
#include <stdio.h>
#include <stdlib.h>


/*          API IMPL        
-------------------------------*/

stack* new_stack(int size) {
    stack* stack_l = (stack*)malloc(sizeof(stack));
    if(size > lim) {
        printf("Stack size too large, expects below 1KB\n");
        return NULL;
    }
    stack_l->table = (void_list)malloc(sizeof(void*) * size);
    stack_l->top = stack_l->table[0];
    stack_l->table[0] = NULL;
    stack_l->size = size;
    stack_l->pos = 0;
    pthread_mutex_init(&stack_l->lock, NULL);
    return stack_l;
}

status destroy(stack* stack_l) {
    int loopv = stack_l->size;
    while(loopv--)
        free(stack_l->table[loopv]);
    free(stack_l);
}

void_list destroy_safe(stack* stack_l) {
    void_list ret = stack_l->table;
    free(stack_l);
}

void* pop(stack* stack_l) {
    if(stack_l == NULL) {
        printf("Stack error: No stack\n");
        return NULL;
    }
    pthread_mutex_lock(&stack_l->lock);
    void* ret = stack_l->top;
    stack_l->top = stack_l->table[stack_l->pos-1];
    stack_l->table[stack_l->pos] = NULL;
    stack_l->pos--;
    pthread_mutex_unlock(&stack_l->lock);
    return ret;
}

void_list pop_n(stack* stack_l, int iter) {
    if(iter > stack_l->size || stack_l == NULL){
        printf("Invalid stack\n");
        return NULL;
    }
    void_list ret = (void_list)malloc(sizeof(void*) * iter);
    if(ret == NULL) {
        perror("malloc");
        printf("Data error\n");
    }
    int loop_v = iter;
    pthread_mutex_lock(&stack_l->lock);
    while(loop_v >= 0) {
        ret[loop_v] = stack_l->table[loop_v];
        stack_l->table[loop_v] = NULL;
        loop_v--;
    }
    stack_l->pos = stack_l->pos - iter;
    if( stack_l->pos != 0 || stack_l->top != NULL )
        stack_l->top = stack_l->table[stack_l->pos];
    pthread_mutex_unlock(&stack_l->lock);
    return ret;
}

status push(stack* stack_l, void* data) {
    if(stack_l == NULL) {
        printf("Stack error: No stack\n");
        return 1;
    }
    if(data == NULL) {
        printf("Stack error: Please provide valid data\n");
        return 1;
    }
    pthread_mutex_lock(&stack_l->lock);
    if(stack_l->pos >= lim || stack_l->pos >= stack_l->size) {
        printf("Stack limit reached, push failed\n");
        return 1;
    }
    stack_l->table[stack_l->pos] = data;
    stack_l->top = stack_l->table[stack_l->pos];
    stack_l->pos++;
    pthread_mutex_unlock(&stack_l->lock);
}

status push_n(stack* stack_l, void_list data, int items) {    
    if(stack_l == NULL) {
        printf("Stack error: No stack\n");
        return 1;
    }
    if(data == NULL) {
        printf("Stack error: Please provide valid data\n");
        return 1;
    }
    if(items > (stack_l->size - stack_l->pos)) {
        printf("Not enough space in the stack\n");
        return 1;
    }

    pthread_mutex_lock(&stack_l->lock);
    items = items -1;
    while(items >= 0) {
        if(stack_l->pos < stack_l->size-1) {
            stack_l->table[stack_l->pos] = data[items];
            stack_l->pos++;
            items--;
        }
    }
    stack_l->top = stack_l->table[stack_l->pos];
    pthread_mutex_unlock(&stack_l->lock);

}

void read_stack(stack* stack_l, const char* format) {
    int point = stack_l->pos-1;
    while(point >= 0) {
        printf(format, stack_l->table[point]);
        point--;
    }
}