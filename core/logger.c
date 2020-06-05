//
// Created by nelson on 6/9/19.
//

#include <stdio.h>
#include <time.h>
#include "hashmap.h"
#include "request.h"

void PrintLog(request_t *Req)
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s]\n", buffer);
}

void Log(request_t *Req){
    FILE *logfile = fopen("log.can", "a");
    if(logfile==NULL){
        printf("Log file creation failed\n");
        return;
    }
    fprintf(logfile,"Request on %d\n", Req->sockfd);
    fprintf(logfile,"\tMethod: %d\n", Req->method);
    fprintf(logfile,"\tHost: %s\n", ht_get(Req->Headers, "Host"));
    fprintf(logfile, "\tConnection: %s\n\n\n",ht_get(Req->Headers, "Connection"));
    fflush(logfile);
    fclose(logfile);
}
