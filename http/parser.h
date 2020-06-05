#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "request.h"
#include "datastructures/stack.h"

/* Sources:
 * https://tomassetti.me/guide-parsing-algorithms-terminology/
 *
 * The algorithms implemented are mostly shift reduce FSMs
 */

#define HTTP_GET 2
#define HTTP_HEAD 3
#define HTTP_POST 4
#define HTTP_PUT 5
#define HTTP_DELETE 6
#define HTTP_MKCOL 7
#define HTTP_COPY 8
#define HTTP_MOVE 9
#define HTTP_OPTIONS 10
#define HTTP_PROPFIND 11
#define HTTP_PROPPATCH 12
#define HTTP_LOCK 13
#define HTTP_UNLOCK 14
#define HTTP_PATCH 15
#define HTTP_TRACE 16

#define HTTP_INVALID_METHOD 10
#define HTTP_INVALID_REQUEST 11
#define HTTP_INVALID_VERSION 12
#define HTTP_REQ_LINE_OK 13
#define HTTP_HEADERS_OK 14
#define HTTP_ARGS_OK 15
#define HTTP_CHUNKS_OK 16
#define HTTP_NO_CATEGORY 17
#define HTTP_CLIENT_ERROR 18

#define HTTP_CONTINUE 100
#define HTTP_SWITCHING_PROTOCOLS 101
#define HTTP_PROCESSING 102

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_ACCEPTED 202
#define HTTP_NO_CONTENT 204
#define HTTP_PARTIAL_CONTENT 206

#define HTTP_SPECIAL_RESPONSE 300
#define HTTP_MOVED_PERMANENTLY 301
#define HTTP_MOVED_TEMPORARILY 302
#define HTTP_SEE_OTHER 303
#define HTTP_NOT_MODIFIED 304
#define HTTP_TEMPORARY_REDIRECT 307
#define HTTP_PERMANENT_REDIRECT 308

#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404
#define HTTP_NOT_ALLOWED 405
#define HTTP_REQUEST_TIME_OUT 408
#define HTTP_CONFLICT 409
#define HTTP_LENGTH_REQUIRED 411
#define HTTP_PRECONDITION_FAILED 412
#define HTTP_REQUEST_ENTITY_TOO_LARGE 413
#define HTTP_REQUEST_URI_TOO_LARGE 414
#define HTTP_UNSUPPORTED_MEDIA_TYPE 415
#define HTTP_RANGE_NOT_SATISFIABLE 416
#define HTTP_MISDIRECTED_REQUEST 421
#define HTTP_TOO_MANY_REQUESTS 429

#define HTTP_INTERNAL_SERVER_ERROR  500
#define HTTP_NOT_IMPLEMENTED 501
#define HTTP_BAD_GATEWAY 502
#define HTTP_SERVICE_UNAVAILABLE 503
#define HTTP_GATEWAY_TIME_OUT 504
#define HTTP_VERSION_NOT_SUPPORTED 505
#define HTTP_INSUFFICIENT_STORAGE 507

int parse_request_line(request_t* r);

int parse_header_lines(request_t* r);

int parse_args(request_t* r);

int parse_chunked(request_t* r);

char* combine_str(stack* s, int size_ml);

char* dup_str(char* p1, char* p2);

#endif // PARSER_H_INCLUDED
