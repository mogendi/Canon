#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "DataStructures/request.h"

void KeyValueDerive(char *HeaderLine,char *Key,char *Value);

void URLParse(request_t *Req);

void HeaderDeriver(request_t *Req, char* EditableMsg);

//The function that parses the http messages into relevant data structures
void HTTPMsgParse(request_t* Req);


#endif // PARSER_H_INCLUDED
