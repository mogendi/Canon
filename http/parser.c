//
// Created by nelson on 6/9/19.
//

#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "datastructures/hashmap.h"
#include <unistd.h>
#include "logger.h"

#define cmp3(m, c0, c1, c2, c3)                                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2
#define cmp4(m, c0, c1, c2, c3)                                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3
#define cmp5(m, c0, c1, c2, c3, c4)                                    \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4
#define cmp6(m, c0, c1, c2, c3, c4, c5)                                \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4 && m[5] == c5
#define cmp7(m, c0, c1, c2, c3, c4, c5, c6, c7)                                \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4 && m[5] == c5 && m[6] == c6
#define cmp8(m, c0, c1, c2, c3, c4, c5, c6, c7)                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7
#define cmp9(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7 && m[8] == c8

#define CR                  '\r'
#define LF                  '\n'


char* quick_scan(char* msg, char delim){
    char *p = msg;
    while(*p != delim && p<msg + strlen(msg)) {
        p+=1;
    }
    if(*p == delim)
        return p;
    return NULL;
}

char* empty_line_search(char* msg) {
    char *p = msg;
    int msg_len = strlen(msg);
    for(p; p<msg + msg_len; p++) {
        if(*p == LF && *(p+1) == CR)
            return p+2;
    }
    return NULL;
}

void dup_str(char* p1, char* p2, char* loc) {
    int size =  (p2-p1)+1;
    int i = 0;
    for(; i<size; i++)
        loc[i] = *(p1 + i);
}

/*           Parser API
 * ---------------------------------
 * */



/* State machine for parsing */
int parse_request_line(request_t* r) {
    typedef enum {
        start_st,
        method_st,
        method_space_st,
        schema_st,
        slash_schema_st,
        slash_slash_schema_st,
        host_start_st,
        host_st,
        host_end_st,
        host_ip_literal_st,
        port_st,
        resource_slash,
        uri_st,
        ver_st,
        ver_h_st,
        ver_ht_st,
        ver_htt_st,
        ver_http_st,
        md_st,
        f_minor_d_st,
        minor_d_st,
        crlf_st,
        space_after_dig_st,
        done_st,
    } state;
    r->req_start = r->MSG;
    char *p;
    char *req_line_end;
    if ((req_line_end = quick_scan(r->MSG, '\r')) == NULL)
        return HTTP_BAD_REQUEST;
    int e = strlen(r->MSG);
    state st = start_st;
    unsigned char c;
    for (p = r->req_start; p < req_line_end; p++) {
        switch (st) {
            case start_st:
                if (*p == ' ') {
                    r->method_end = p - 1;
                    int meth_size = p - r->req_start;
                    switch (meth_size) {
                        case 3:
                            if (cmp3(r->MSG, 'G', 'E', 'T', ' ')) {
                                r->method = HTTP_GET;
                                break;
                            }
                            if (cmp3(r->MSG, 'P', 'U', 'T', ' ')) {
                                r->method = HTTP_PUT;
                                break;
                            }
                            break;
                        case 4:
                            if (cmp4(r->MSG, 'P', 'O', 'S', 'T')) {
                                r->method = HTTP_POST;
                                break;
                            }

                            if (cmp4(r->MSG, 'C', 'O', 'P', 'Y')) {
                                r->method = HTTP_COPY;
                                break;
                            }

                            if (cmp4(r->MSG, 'M', 'O', 'V', 'E')) {
                                r->method = HTTP_MOVE;
                                break;
                            }

                            if (cmp4(r->MSG, 'L', 'O', 'C', 'K')) {
                                r->method = HTTP_LOCK;
                                break;
                            }

                            if (cmp4(r->MSG, 'H', 'E', 'A', 'D')) {
                                r->method = HTTP_HEAD;
                                break;
                            }
                        case 5:
                            if (cmp5(r->MSG, 'M', 'K', 'C', 'O', 'L')) {
                                r->method = HTTP_MKCOL;
                                break;
                            }
                            if (cmp5(r->MSG, 'P', 'A', 'T', 'C', 'H')) {
                                r->method = HTTP_PATCH;
                                break;
                            }
                            if (cmp5(r->MSG, 'T', 'R', 'A', 'C', 'E')) {
                                r->method = HTTP_TRACE;
                                break;
                            }
                            break;
                        case 6:
                            if (cmp6(r->MSG, 'D', 'E', 'L', 'E', 'T', 'E')) {
                                r->method = HTTP_DELETE;
                                break;
                            }
                            if (cmp6(r->MSG, 'U', 'N', 'L', 'O', 'C', 'K')) {
                                r->method = HTTP_UNLOCK;
                                break;
                            }
                            break;
                        case 7:
                            if (cmp7(r->MSG, 'O', 'P', 'T', 'I', 'O', 'N', 'S', ' ')) {
                                r->method = HTTP_OPTIONS;
                                break;
                            }
                            break;
                        case 8:
                            if (cmp8(r->MSG, 'P', 'R', 'O', 'P', 'F', 'I', 'N', 'D')) {
                                r->method = HTTP_PROPFIND;
                                break;
                            }
                            break;
                        case 9:
                            if (cmp9(r->MSG, 'P', 'R', 'O', 'P', 'P', 'A', 'T', 'C', 'H')) {
                                r->method = HTTP_PROPPATCH;
                                break;
                            }
                            break;
                        default:
                            return HTTP_INVALID_METHOD;
                    }
                    st = method_space_st;
                    break;
                }
                if ((*p < 'A' || *p > 'Z') && *p != '_' && *p != '-')
                    return HTTP_INVALID_METHOD;
            case method_space_st:
                if (*p == '/') {
                    r->uri_start = p;
                    st = resource_slash;
                    break;
                }
                if (*p >= 'a' && *p <= 'z') {
                    r->schema_start = p;
                    st = schema_st;
                    break;
                }
                if (*p == ' ')
                    break;
                else { return HTTP_BAD_REQUEST; }
            case schema_st:
                c = (unsigned char) (*p | 0x20);
                if (c >= 'a' && c <= 'z') break;
                if ((*p >= '0' && *p <= '9') || *p == '+' || *p == '-' || *p == '.') break;
                switch (*p) {
                    case ':':
                        r->schema_end = p - 1;
                        st = slash_schema_st;
                        break;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
                break;
            case slash_schema_st:
                switch (*p) {
                    case '/':
                        st = slash_slash_schema_st;
                        break;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
                break;
            case slash_slash_schema_st:
                switch (*p) {
                    case '/':
                        st = host_start_st;
                        break;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
                break;
            case host_start_st:
                r->host_start = p - 1;
                if (*p == '[') {
                    st = host_ip_literal_st;
                    break;
                }
                st = host_st;
            case host_st:
                c = (unsigned char) (*p | 0x20);
                if (c >= 'a' && c <= 'z') break;
                if ((*p >= '0' && *p <= '9') || *p == '.' || *p == '-') break;
            case host_end_st:
                r->host_end = p - 1;
                switch (*p) {
                    case ':':
                        st = port_st;
                        break;
                    case '/':
                        r->uri_start = p;
                        st = resource_slash;
                        break;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
                break;
            case host_ip_literal_st:
                if (*p >= '0' && *p <= '9') {
                    break;
                }

                c = (unsigned char) (*p | 0x20);
                if (c >= 'a' && c <= 'z') {
                    break;
                }

                switch (*p) {
                    case ':':
                        break;
                    case ']':
                        st = host_end_st;
                        break;
                    case '~':
                        break;
                    case '=':
                        break;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
                break;
            case port_st:
                if (*p >= '0' && *p <= '9') {
                    break;
                }

                switch (*p) {
                    case '/':
                        r->port_end = p;
                        r->uri_start = p;
                        st = resource_slash;
                        break;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
                break;
            case resource_slash:
                switch (*p) {
                    case ' ':
                        r->uri_end = p;
                        st = ver_st;
                        break;
                    case '?':
                        r->complex_uri = 1;
                        r->has_query = 1;
                        r->args_start = p;
                        break;
                    case '#':
                        r->has_fragment = 1;
                        r->complex_uri = 1;
                        r->args_start = p;
                }
                break;
            case ver_st:
                switch (*p) {
                    case 'h':
                        st = ver_h_st;
                        break;
                    case 't':
                        st = ver_ht_st;
                        p += 1;
                        if ((*p) == 't')
                            st = ver_htt_st;
                        else { return HTTP_BAD_REQUEST; }
                    case 'p':
                        st = ver_http_st;
                        break;
                    case '/':
                        st = md_st;
                        break;
                    default:
                        return HTTP_BAD_REQUEST;
                }
                break;
            case md_st:
                if (*p < '1' || *p > '9')
                    return HTTP_INVALID_REQUEST;
                r->http_major = *p - '0';
                if (r->http_major > 2)
                    return HTTP_INVALID_VERSION;
                st = f_minor_d_st;
                break;
            case f_minor_d_st:
                if ((*(p + 1) < '1' || *(p + 1) > '9') && *p != '.')
                    return HTTP_INVALID_REQUEST;
                st = minor_d_st;
                break;
            case minor_d_st:
                r->http_minor = *p + '0';
                if (r->http_minor > 99)
                    return HTTP_INVALID_REQUEST;
                st = crlf_st;
                r->ver = (r->http_major *100) + r->http_minor;
                break;
            case crlf_st:
                if (*p == CR) {
                    st = done_st;
                    break;
                } else if (*p == ' ') {
                    st = space_after_dig_st;
                    break;
                } else if (*p == LF)
                    goto done;
                else { return HTTP_INVALID_REQUEST; }
            case space_after_dig_st:
                switch (*p) {
                    case ' ':
                        break;
                    case CR:
                        st = done_st;
                        break;
                    case LF:
                        goto done;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
                break;
            case done_st:
                r->req_end = p;
                switch (*p) {
                    case LF:
                        goto done;
                    default:
                        return HTTP_INVALID_REQUEST;
                }
            default:
                return HTTP_BAD_REQUEST;
        }
    }
done:
    if (r->req_end == NULL)
        r->req_end = p;
    r->ver = r->http_major * 10 + r->http_minor;
    return HTTP_REQ_LINE_OK
}

int parse_header_line(request_t* r) {
    typedef enum {
        start_st,
        value_st,
        space_st,
        empty_line,
        almost_done_line,
        done_line_st,
        done_headers_st
    }state;
    state st = start_st;
    char* headers_end = empty_line_search(r->MSG);
    r->headers_start = r->req_end+1;
    r->headers_end = headers_end;
    char *p = r->headers_start;
    char *name_b, *name_e, *value_b, *value_e;
    char *name_l, *value_l;
    name_b = r->headers_start;
    for(p; p<r->headers_end; p++){
        if(p == headers_end)
            return HTTP_BAD_REQUEST;
        switch(st){
            case start_st:
                if(*p == ' ')
                    return HTTP_BAD_REQUEST;
                if(*p == ':'){
                    name_e = p-1;
                    char name[(name_e-name_b)+1];
                    dup_str(name_b, name_e, name);
                    name_l = name;
                }
                st = space_st;
                break;
            case space_st:
                if(*p == CR) {
                    st = almost_done_line;
                    break;
                }
                if(*p == ' ' && *(p+1) == ' ')
                    return HTTP_BAD_REQUEST;
                if(*p == ' ')
                    value_b = p +1;
                else { value_b = p; }
                st = value_st;
            case value_st:
                switch(*p) {
                    case CR:
                        if((*(p-1) == ' ' && *(p-2) == ' ' ) || *(p+1) != LF)
                            return HTTP_BAD_REQUEST;
                }
                st = done_line_st;
            case almost_done_line:
                if(*p+1 != LF)
                    return HTTP_BAD_REQUEST;
                name_b = p-1;
                st = done_line_st;
            case done_line_st:
                value_e = p-2;
                int size = (value_e-value_b) +1;
                char value[size];
                dup_str(value_b, value_e, value);
                value_l = value;
                name_b = p+1;
                goto done_line;
        }
done_line:
            ht_set(r->Headers, name_l, value_l);
            st = start_st;
    }

    return HTTP_BAD_REQUEST;
}