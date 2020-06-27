//
// Created by nelson on 5/2/19.
//

/*
 * Implementation of the parsing logic
 * */

/*#include <stdio.h>
#include <stdlib.h>
#include "datastructures/queue.h"
#include "Engine/canon_tpool.h"
#include "request.h"
#include "mutex.h"
#include "connection.h"
#include "parser.h"*/

/*int main(){
    int port_c;
    thpool_t* pool = pool_init((void*)HTTPMsgParse);
    int loopv = 100;

    sock channel = HTTPConnectionGen(8080);
    socklen_t port_l = sizeof(channel.port);
    while(loopv){
        if(listen(channel.sock_fd,4)<0)
            printf("failed listen");
        if( (port_c = accept(channel.sock_fd, (struct sockaddr *)&channel.port, &port_l)) <0 )
            perror("Failed Accept");
        request_t* vrequest = create_request(port_c);
        HTTPMsgTransfer(vrequest,0,NULL);
        Enqueue(vrequest, pool->job_queue);
        loopv--;
    }
    pool_kill(pool);
}*/
