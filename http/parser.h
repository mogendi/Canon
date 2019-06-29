#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "DataStructures/request.h"

char* get_url(request_t* Req);

char* get_body(request_t* Req);

void urlparse(request_t *Req);

void headerparse(request_t *Req, char* EditableMsg);

//The function that parses the http messages into relevant data structures
void HTTPMsgParse(request_t* Req);


#endif // PARSER_H_INCLUDED
