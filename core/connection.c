//
// Created by nelson on 6/5/20.
//

#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define DEFSIZE 1024

//Low level message transfer function. Expects data in its
//proper format.

int rcv(request_t* r){
    ssize_t ret;
    char* rcv_buf = malloc(DEFSIZE);
    ret = recv(r->sockfd, rcv_buf, DEFSIZE, 0);
    if(ret == 0)
        shutdown(r->sockfd, SHUT_RD);
    if(ret == -1)
        shutdown(r->sockfd, SHUT_RDWR);
    if(ret > 0)
        r->MSG = rcv_buf;
    return ret;
}

int snd(request_t* r){ //It's assumed that the data being sent is r->resp
    if(r->resp == NULL)
        return -1;
    char* off = r->resp;
    int len = r->bl, i = 0;
    while(len > 0){
        i = send(r->sockfd, r->resp, r->bl, 0);
        if(i<1){
            perror("send");
            return i;
        } else {
            len -= i;
            off += i;
        }
    }
    return i;
}

char* HTTPMsgTransfer(request_t *Req, int Flag, char* buff){
    ssize_t bufsize = 0;
    char* rcv_buf;
    if(Flag == RECV){
        rcv_buf = malloc(DEFSIZE + 2);
        bufsize = recv(Req->sockfd, rcv_buf, DEFSIZE+2, 0);
        if (bufsize <= 0) {
            perror("recv");
            printf("Corrupt Request: %ld\n", bufsize);
        }
        if (bufsize > 0) {
            return rcv_buf;
        }
    }

    if(Flag == SEND) {
        if (buff == NULL) {
            printf("Send requires an input buff");
            return NULL;
        }
        int a = send(Req->sockfd, buff, Req->bl, 0);
        return NULL;
    }
    return NULL;
}

//returns an integer socket_fd bound to an address
sock* HTTPConnectionGen(int PORT){
    int connection, opt = 1;
    struct sockaddr_in conn_addr;

    if((connection = socket(AF_INET, SOCK_STREAM, 0))<1)
        perror("SOCKET CREATION FAILED");

    if(setsockopt(connection, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt))<0)
        perror("SOCKET OPTIONS NOT GRANTED");

    conn_addr.sin_family = AF_INET;
    conn_addr.sin_addr.s_addr = INADDR_ANY;
    conn_addr.sin_port = htons(PORT);

    if(bind(connection, (const struct sockaddr *)&conn_addr, sizeof(conn_addr))<0)
        perror("BIND FAILED");

    sock* conn = malloc(sizeof(sock));
    conn->sock_fd = connection;
    conn->port = conn_addr;

    return conn; // more like connection channel
}