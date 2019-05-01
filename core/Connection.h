//
// Created by nelson on 5/1/19.
//

#ifndef CANON_CONNECTION_H
#define CANON_CONNECTION_H

#include <sys/socket.h>
#include<netinet/in.h>
#include "request.h"
#define RECV 0
#define SEND 1



/*
 * HTTP connection semantics(RFC 7230:50) and HTTP/TLS not implemented
 * */

//Low level message transfer function. Expects data in its
//proper format.
void HTTPMsgTransfer(request_t *Req, int Flag, char* buff){
    char recv_buff[1024];
    if(Flag == RECV){
        recv(Req->sockfd, recv_buff, sizeof(recv_buff),0);
        Req->MSG = buff;
    }

    if(Flag == SEND) {
        if (buff == NULL) {
            printf("Send requires an input buff");
            return;
        }
        send(Req->sockfd, buff, sizeof(buff), 0);
    }
}


void HTTPConnectionGen(int PORT){
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

    if (listen(connection, 4) < 0)
        perror("FAILED LISTEN");
}


#endif //CANON_CONNECTION_H
