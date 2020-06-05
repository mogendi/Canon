//
// Created by nelson on 3/2/20.
//

#ifndef CANON_HANDLER_H
#define CANON_HANDLER_H

/*
 * Package the parse functions so that a response (code+headers+body)
 * can be generated.
 * Generate responses from parse literals.
 *
 * */

#include "parser.h"

typedef struct {
    char *name;
    char *value;
}response_header_temp;

typedef struct {
    char* ver;
    char* code;
    char *reason;
}status_line;

status parse_routine(request_t* r);

status handler(char* raw, int sockfd);


#endif //CANON_HANDLER_H
