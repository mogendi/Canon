//
// Created by nelson on 6/9/19.
//

#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
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

#define CR '\r'
#define LF '\n'


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

char* dup_str(char* p1, char* p2) {
    int size_ml = (p2 - p1) + 2;
    char *loc = (char*)malloc(sizeof(char)*size_ml);
    int size =  (p2-p1)+1;
    int i = 0;
    for(; i<=size; i++) {
        *(loc + i) = *(p1 + i);
        if (i == size)
            loc[i] = '\0';
    }
    return loc;
}

char* combine_str(stack* s, int size_ml){
    char *str = (char*)malloc((sizeof(char)*size_ml)+1);
    if(str == NULL)
        return NULL;
    char *i = &str[size_ml-1], *j; int x = size_ml-1;
    char *sb = NULL, *se  = NULL;
    str[size_ml] = '\0';
    for(; i>str; ){
        se = pop(s);
        sb = pop(s);
        j = se;
        for(; j>=sb; j--){
            *(str+x) = *j;
            x--;
        }
        i = &str[x] - 1;
    }
    return str;
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
                break;
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
                r->host_start = p;
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
                if(*(p-1) == ':')
                    r->port_start = p;
                switch (*p) {
                    case '/':
                        r->port_end = p-1;
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
                        r->args_end = p;
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
                    case 'H':
                        st = ver_h_st;
                        p+=1;
                    case 'T':
                        st = ver_ht_st;
                        p += 1;
                        if ((*p) == 'T')
                            st = ver_htt_st;
                        else { return HTTP_BAD_REQUEST; }
                    case 'P':
                        st = ver_http_st;
                        p+=1;
                    case '/':
                        st = md_st;
                        break;
                    default:
                        return HTTP_BAD_REQUEST;
                }
                break;
            case md_st:
                p+=1;
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
                r->http_minor = *p - '0';
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
        }
    }
done:
    if (r->req_end == NULL)
        r->req_end = p;
    r->ver = r->http_major * 10 + r->http_minor;
    return HTTP_REQ_LINE_OK;
}

int parse_header_lines(request_t* r) {
    typedef enum {
        start_st,
        value_st,
        space_st,
        almost_done_line,
        done_line_st,
    }state;
    state st = start_st;

    char* headers_end;
    if(r->trailers == 1){
        headers_end = empty_line_search(r->trailers_start);
    } else { headers_end = empty_line_search(r->MSG);}
    if(headers_end == NULL)
        return HTTP_BAD_REQUEST;

    if(*(headers_end+1) != NULL) {
        r->body = headers_end+1;
    }

    char *p, *name_b, *lim_t;
    if(!r->trailers) {
        r->headers_start = r->req_end + 2;
        r->headers_end = headers_end;
        p = r->headers_start;
        name_b = r->headers_start;
        lim_t = r->headers_end;
    } else {
        r->trailers_end = headers_end;
        p = r->trailers_start;
        name_b = r->trailers_start;
        lim_t = r->trailers_end;
    }

    char *name_e = NULL, *value_b = NULL, *value_e = NULL;
    char *name_l, *value_l;

hl_loop:
    for(; p<lim_t; p++){
        switch(st){
            case start_st:
                if(*p == ' ')
                    return HTTP_BAD_REQUEST;
                if(*p == ':'){
                    name_e = p-1;
                    st = space_st;
                }
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
                st = value_st;
                break;
            case value_st:
                switch(*p) {
                    case CR:
                        if((*(p-1) == ' ' && *(p-2) == ' ' ) || *(p+1) != LF)
                            return HTTP_BAD_REQUEST;
                        value_e = p-1;
                    case LF:
                        st = done_line_st;
                }
                break;
            case almost_done_line:
                if(*p+1 != LF)
                    return HTTP_BAD_REQUEST;
                name_b = p-1;
                st = done_line_st;
                break;
            case done_line_st:
                if((headers_end - p) <= 4) {
                    goto done_headers;
                } else { goto done_line; }
        }

    }

done_line:
    name_l = dup_str(name_b, name_e);
    value_l = dup_str(value_b, value_e);
    ht_set(r->Headers, name_l, value_l);
    st = start_st;
    name_b = p + 1;
    goto hl_loop;

done_headers:
    name_l = dup_str(name_b, name_e);
    value_l = dup_str(value_b, value_e);
    ht_set(r->Headers, name_l, value_l);
    r->body = r->headers_end;
    return HTTP_HEADERS_OK;
}

int parse_args(request_t* r){
    typedef enum {
        start_st,
        query_st,
        q_args_st,
    }state;

    char *value_st = NULL, *query_str = NULL, *args_st = NULL;
    char *value_end = NULL, *query_end = NULL, *args_end = NULL;
    if(r->complex_uri != 1)
        return HTTP_NO_CATEGORY;
    value_st = r->uri_start+1;
    query_str = r->args_start+1;
    char *p = r->uri_start+1;
    r->args->len = 0;
    state st = start_st;
    args_t* arg_cur = r->args;

    for(; p<=r->args_end; p++){
        switch(st) {
            case start_st:
                if(*p == '/'){
                    value_end = p-1;
                    arg_cur->len++;
                    arg_cur->lvalue = dup_str(value_st, value_end);
                    value_st = p+1;
                    arg_cur->chain = (args_t*)malloc(sizeof(args_t));
                    arg_cur = arg_cur->chain;
                    break;
                }
                if(*p == '?' || *p == '#'){
                    arg_cur->lvalue = dup_str(value_st, (p-1));
                    st = query_st;
                    break;
                }
                if(*p == '='){
                    value_end = p-1;
                    arg_cur->len++;
                    arg_cur->query = dup_str(value_st, value_end);
                    args_st = p+1;
                    st = q_args_st;
                    break;
                }
                break;
            case query_st:
                if(*p == '='){
                    query_end = p-1;
                    arg_cur->query = dup_str(query_str, query_end);
                    args_st = p+1;
                    st = q_args_st;
                    break;
                }
                if(*p == ' ' && p == r->uri_end)
                    goto done_args;
                break;
            case q_args_st:
                if(*p == '&'){
                    args_end = p-1;
                    query_str = p+1;
                    arg_cur->query_arg = dup_str(args_st, args_end);
                    arg_cur->chain = (args_t*)malloc(sizeof(args_t));
                    arg_cur = arg_cur->chain;
                    st = query_st;
                    break;
                }
                if(*p == ' ' && p == r->args_end){
                    args_end = p-1;
                    arg_cur->query_arg = dup_str(args_st, args_end);
                    goto done_args;
                }
        }
    }
done_args:
    return HTTP_ARGS_OK;
}

int parse_chunked(request_t* r) {
    if(ht_get(r->Headers, "transfer-coding") == NULL)
        return HTTP_NO_CATEGORY;

    typedef enum {
        start_st,
        size_st,
        ext_q_st,
        ext_a_st,
        data_st,
        lc_st,
        trailers_st,
        end_line_st,
    }state;

    int lc_flag = 0;

    char *args_b = NULL, *data_b = NULL;
    char *args_e = NULL, *data_e = NULL;
    char *q_b = NULL, *q_e = NULL;
    char *curr_p = NULL;
    int dsize = 0;
    char *p = r->headers_end;
    if(p == NULL || *p == '\0')
        return HTTP_NO_CATEGORY;
    state st = start_st;
    char ch;
    stack *str_= new_stack(lim);
    exts_t* curr = r->chunks->ext;
    char* chunks_end  = empty_line_search(p);
    while(p<chunks_end){
        p = p+1;
        switch(st){
            case start_st:
                if(*p >= '0' && *p <= '9'){
                    dsize= dsize + (*p - '0');
                    st = size_st;
                    break;
                }
                ch = (*p | 0x20);
                if(ch >='a' && ch<='f'){
                    dsize = dsize + (ch - 'a' + 10);
                    st = size_st;
                    break;
                }
                return HTTP_CLIENT_ERROR;
            case size_st:
                if(*p >= '0' && *p <= '9'){
                    dsize = dsize*16 + (*p - '0');
                    break;
                }
                ch = (*p | 0x20);
                if(ch >='a' && ch<='f'){
                    dsize = dsize*16 + (ch - 'a' + 10);
                    break;
                }
                if(*p == ' ')
                    break;
                if(*p == ';'){
                    if(*(p+1) != ' ')
                        q_b = p+1;
                    else {
                        q_b = p+2;
                    }

                    r->chunks->chunk_size += dsize;
                    st = ext_q_st;
                    break;
                }
                if(*p == CR && *(p+1) == LF){
                    r->chunks->chunk_size += dsize;
                    curr_p = p+(dsize-1)+2;
                    data_b = p+2;
                    st = data_st;
                    break;
                }
            case ext_q_st:
extension:
                if(*p == '='){
                    q_e = p-1;
                    curr->query = dup_str(q_b, q_e);
                    args_b = p+1;
                    st = ext_a_st;
                    break;
                }
                if(*p == CR && *(p+1) == LF){
                    curr->ext = (exts_t*)malloc(sizeof(exts_t));
                    curr = curr->ext;
                    if(st == lc_st)
                        goto last_chunk;
                    st = data_st;
                    break;
                }
                break;
            case ext_a_st:
                switch(*p){
                    case CR:
                        if (*(p+1) == LF) {
                            args_e = p - 1;
                            curr->query_arg = dup_str(args_b, args_e);
                            curr->ext = (exts_t *) malloc(sizeof(exts_t));
                            curr = curr->ext;
                            break;
                        }
                    case ' ':
                        break;
                    case LF:
                        data_b = p+1;
                        curr_p = p + (dsize);
                        if(lc_flag)
                            goto last_chunk;
                        st = data_st;
                        break;
                }
                break;
            case data_st:
                if(p<curr_p)
                    break;
                if(p == curr_p && *(p+1) == CR){
                    data_e = p;
                    push(str_, data_b);
                    push(str_, data_e);
                    if(*(p+3) == '0'){
                        st = lc_st;
                        lc_flag = 1;
                        break;
                    }
                    dsize = 0;
                    p+=2;
                    st = start_st;
                    break;
                }
            case lc_st:
                if(*p != '0')
                    break;
                if(*p == ';'){
                    if(*(p+1) != ' ')
                        q_b = p+1;
                    else { break; }

                    goto extension;
                }
                if(*p == '0') {
                    p += 1;
                    goto last_chunk;
                }

last_chunk:
                if(*p == CR){
                    if(*(p+2) == CR) {
                        p = p+2;
                        st = end_line_st;
                        break;
                    } else {
                        st = trailers_st;
                        break;
                    }
                }
                break;
            case trailers_st:
                r->trailers = 1;
                r->trailers_start = p+1;
                r->trailers_end = empty_line_search(r->trailers_start);
                parse_header_lines(r);
                goto done_chunks;

            case end_line_st:
                if(*p == CR && *(p+1) == LF && (p+1) == chunks_end)
                    goto done_chunks;
                break;

            default:
                break;
        }
    }

done_chunks:
    r->chunks->body = combine_str(str_, r->chunks->chunk_size);
    r->body = r->chunks->body;
    return HTTP_CHUNKS_OK;
}