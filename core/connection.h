//
// Created by nelson on 5/1/19.
//

#ifndef CANON_CONNECTION_H
#define CANON_CONNECTION_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "request.h"
#define RECV 0
#define SEND 1

/*
 * http connection semantics(RFC 7230:50) and http/TLS not implemented
 * */

struct sock_p{
    int sock_fd;
    struct sockaddr_in port;
};

typedef struct sock_p sock;
int rcv(request_t* r);
int snd(request_t* r);
char* HTTPMsgTransfer(request_t *Req, int Flag, char* buff);
sock* HTTPConnectionGen(int PORT);


#endif //CANON_CONNECTION_H