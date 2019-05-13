#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DataStructures/hashmap.h"
#include "DataStructures/request.h"
#include <unistd.h>
#include "FileOps/logger.h"

void KeyValueDerive(char *HeaderLine,char *Key,char *Value){
    int i = 0, j = 0;
    if(HeaderLine == NULL)
        return;
    char x = HeaderLine[0];
    while(x!=':'){
        Key[i] = HeaderLine[i];
        i++;
        //printf("%d\n", i);
        x = HeaderLine[i];
    }

    Key[i] = NULL;
    i = i+2;
    while(x!='\0'){
        Value[j] = HeaderLine[i];
        j++;i++;
        x = HeaderLine[i];
    }
    Value[j] = NULL;
    i=0;j=0;
}

void URLParse(request_t *Req){
    char *ReqlEd = strdup(Req->req_line);
    Req->method = strtok(ReqlEd, " ");
    Req->URL = strtok(NULL, " ");
    PrintLog(Req);
    Log(Req);
}

void HeaderDeriver(request_t *Req, char* EditableMsg){
    char delim[] = "\r\n";
    int Size = getpagesize();
    hashtable_t *HeaderFields = ht_create(Size/2);

    char *req_line = strtok(EditableMsg, delim);//Request Line
    Req->req_line = req_line;

    if(HeaderFields == NULL)
        printf("Failed to create hash map\n");

    char *hf = strtok(NULL, delim);//Host Field

    if(hf == NULL || (strcmp(hf, ""))==0){
            //ErrorRaiser(400, BWS);
            printf("\nTrue\n");
        }
    char field[30], hvalue[120];
    KeyValueDerive(hf, field, hvalue);

    if(hf == NULL)
        printf("NULL Value");
    while((hf = strtok(NULL, delim)) != NULL){
        if(strcmp(hf, "\177")==0)
            break;
        ht_set(HeaderFields, field, hvalue);
        KeyValueDerive(hf, field, hvalue);
    }
    Req->Headers = HeaderFields;
    URLParse(Req);
}

//The function that parses the http messages into relevant data structures
void HTTPMsgParse(request_t* Req){
    if(strlen(Req->MSG)==0){
        /* Supposed to return a bad request response
         * Unimplemented as of now
         * */
        printf("Empty message sent\n");
        kill_Req(Req);
        return;
    }
    char *EditableMSG = strdup(Req->MSG);
    HeaderDeriver(Req, EditableMSG);
}


#endif // PARSER_H_INCLUDED
