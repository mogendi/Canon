//
// Created by nelson on 5/1/19.
//

#ifndef CANON_CONNECTION_H
#define CANON_CONNECTION_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
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
//Low level message transfer function. Expects data in its
//proper format.
void HTTPMsgTransfer(request_t *Req, int Flag, char* buff){
    char recv_buff[1024];
    ssize_t buffsize;
    if(Flag == RECV){
        buffsize = recv(Req->sockfd, recv_buff, sizeof(recv_buff), 0);
        if (buffsize == 0 || buffsize == -1) {
            printf("Corrupt Request");
        }
        if (buffsize > 0) {
            Req->MSG = strdup(recv_buff);
        }
    }

    if(Flag == SEND) {
        if (buff == NULL) {
            printf("Send requires an input buff");
            return;
        }
        send(Req->sockfd, buff, sizeof(buff), 0);
    }
}

//returns an integer socket_fd bound to an address
sock HTTPConnectionGen(int PORT){
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

    sock conn;
    conn.sock_fd = connection;
    conn.port = conn_addr;

    return conn; // more like connection channel
}


#endif //CANON_CONNECTION_H
