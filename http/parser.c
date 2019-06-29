//
// Created by nelson on 6/9/19.
//

#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DataStructures/hashmap.h"
#include <unistd.h>
#include "logger.h"

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

    Key[i] = '\0';
    i = i+2;
    while(x!='\0'){
        Value[j] = HeaderLine[i];
        j++;i++;
        x = HeaderLine[i];
    }
    Value[j] = '\0';
    i=0;j=0;
}


/*           Parser API
 * ---------------------------------
 * */
void urlparse(request_t *Req){
    char *ReqlEd = strdup(Req->req_line);
    Req->method = strtok(ReqlEd, " ");
    Req->URL = strtok(NULL, " ");
    PrintLog(Req);
    Log(Req);
}

void headerparse(request_t *Req, char* EditableMsg){
    const char delim[] = "\r\n";
    const int Size = getpagesize()/8;
    hashtable_t *HeaderFields = ht_create(Size/2);

    char *req_line = strtok(EditableMsg, delim);//Request Line
    Req->req_line = req_line;

    if(HeaderFields == NULL)
        printf("Failed to create hash map\n");

    char *hf = strtok(NULL, delim);//Host Field

    if(hf == NULL || (strcmp(hf, ""))==0){
        //ErrorRaiser(400, BWS);
        printf("\nEmpty Message: Exiting Parse\n");
        return;
    }
    char field[30], hvalue[120];
    KeyValueDerive(hf, field, hvalue);

    while((hf = strtok(NULL, delim)) != NULL){
        if(strcmp(hf, "\177")==0)
            break;
        ht_set(HeaderFields, field, hvalue);
        KeyValueDerive(hf, field, hvalue);
    }
    Req->Headers = HeaderFields;
    urlparse(Req);
}

void HTTPMsgParse(request_t* Req){
    (request_t*)Req;
    if(strlen(Req->MSG)==0){
        /* Supposed to return a bad request response
         * Unimplemented as of now
         * */
        printf("Empty message sent\n");
        kill_Req(Req);
        return;
    }

    char *EditableMSG = strdup(Req->MSG);
    int size = 0; char c = 'a';
    while(c != '\0') {
        c = EditableMSG[size];
        size++;
    }
    if(size <= 0 || EditableMSG == NULL) {
        printf("Size: %d\n", size);
        printf("\n\t strdup() failed \n");
        fflush(stdout);
    }

    headerparse(Req, EditableMSG);
}