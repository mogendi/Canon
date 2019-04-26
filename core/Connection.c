#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<sys/sysinfo.h>
#include <unistd.h>
#include "Parser.h"
#include "request.h"
#include "canon_tpool.c"
#include <sys/time.h>
#include <sys/select.h>

#define RECV 0
#define SEND 1


void *ThreadEntryPoint(void* HTTPConnection);


//Low level message transfer function. Expects data in its
//proper format.
void HTTPMsgTransfer(request_t *Req, int Flag){
    char buff[1024];
    if(Flag == RECV){
        recv(Req->sockfd, buff, sizeof(buff),0);
        Req->MSG = buff;
        HTTPMsgParse(Req);}

    if(Flag == SEND)
        write(Req->sockfd, buff, sizeof(buff));
}

void HTTPConnectionGen(char* ADDR, int PORT){
    int connection, opt = 1, new_socket, dummy /*THREADS = get_nprocs()*/;
    struct sockaddr_in conn_addr;
    struct timeval tv;
    fd_set readfds;
    FD_ZERO(&readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 30;

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

    socklen_t structsize = sizeof(conn_addr);

    while(1){

        if ((new_socket = accept(connection, (struct sockaddr *)&conn_addr, &structsize))<0)
            perror("FAILED ACCEPT");

        FD_SET(new_socket, &readfds);
        if(select(new_socket+1, &readfds, NULL, NULL, &tv) < 0){
            if ((dummy = accept(connection, (struct sockaddr *)&conn_addr, &structsize))>0) {
                printf("Reconnection Caught \n");
                shutdown(dummy, 2);
            }
        }
        pthread_t threadid;
        pthread_create(&threadid, NULL, ThreadEntryPoint, (void *)new_socket);
        printf("ThreadID: %ld\tSocketFD: %d\n", threadid, new_socket);
    }
}

//The function called by the thread creator
void* ThreadEntryPoint(void* HTTPConn){
    request_t* vrequest = createRequest(NULL, NULL, NULL, NULL, NULL, NULL, (int)HTTPConn);
    HTTPMsgTransfer(vrequest, RECV);
}

int main(){
    char addr[] = "127.0.0.1";
    int port = 8080;
    HTTPConnectionGen(addr, port);
}
