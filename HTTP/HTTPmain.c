//
// Created by nelson on 5/2/19.
//

/*
 * Implementation of the parsing logic
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include "DataStructures/queue.h"
#include "Engine/canon_tpool.h"
#include "DataStructures/request.h"
#include "mutex.h"
#include "Connection.h"
#include "Parser.h"

int main(){
    int work_pool_size = get_nprocs(), port_c;
    queue* Q = CreateQ();
    thpool_t* pool = pool_init((work_pool_size),Q, HTTPMsgParse);


    sock channel = HTTPConnectionGen(8080);
    socklen_t port_l = sizeof(channel.port);
    while(1){
        if(listen(channel.sock_fd,4)<0)
            printf("failed listen");
        if( (port_c = accept(channel.sock_fd, (struct sockaddr *)&channel.port, &port_l)) <0 )
            perror("Failed Accept");
        request_t* vrequest = createRequest(NULL, NULL, NULL, NULL, NULL, NULL, port_c);
        HTTPMsgTransfer(vrequest,0,NULL);
        Enqueue(vrequest, Q);
    }
    pool_kill(pool);
}
