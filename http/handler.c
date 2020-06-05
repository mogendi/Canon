//
// Created by nelson on 3/1/20.
//

#include "handler.h"
#include "../core/can_file.h"
#include "../core/connection.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define atoa(x) #x

char* dt(){
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char * s = (char*)malloc(64 * sizeof(char));
    assert(strftime(s, (64 * sizeof(char)), "%c", tm));
    printf("%s\n", s);
    return s;
}

int sl_flag = 0, rht_flag = 0;
char* Server = "Canon";

static response_header_temp rht[] = {
        {"Content-Length", NULL},
        {"Content-Type", NULL},
        {"Date", NULL},
        {"Last_modified", "0"},
        {"Server", NULL},
        {"Set-Cookie", "0"},
        {"Vary", "0"},
        {"ETag", "0"},
        {"Transfer-Encoding", "0"},
        {"Content-Encoding", "0"}
};

static status_line sl = {
        "HTPP/1.1", "0", "0"
};

size_t hd_size(response_header_temp* rh, status_line* sl_t) {
    int sz=0, i=0;
    if(rh == NULL){
        if(sl_t == NULL){
            return 0;
        }
        sz = strlen(sl_t->ver) + strlen(sl_t->reason) + strlen(sl_t->code);
    } else {
        if(sl_t != NULL)
            sz += strlen(sl_t->ver) + strlen(sl_t->reason) + strlen(sl_t->code);;
        for(;i<10;i++){
            sz += strlen(rh[i].name) + strlen(rh[i].value);
        }
    }
    return sz;
}

char* write_msg(char* body){
    //Struct to string conversion

    char *msg = (char*)malloc(hd_size(rht, &sl));
    if(!sl_flag)
        return NULL;
    sprintf(msg, "%s %s %s\r\n"
                 "%s: %s\r\n"
                 "%s: %s\r\n"
                 "%s: %s\r\n"
                 "%s: %s\r\n\r\n"
                 "%s",
                  sl.ver, sl.code, sl.reason,
                  rht[0].name, rht[0].value,
                  rht[1].name, rht[1].value,
                  rht[2].name, rht[2].value,
                  rht[4].name, rht[4].value, body);
    return msg;
}

char* body_gen(request_t* r, char* body){
    //config is tied to /var/local/
    //looks for index.html if no url given
    char* append;
    char sz_str[10], cnt_type[15];

    if(body != NULL) {
        size_t sz = strlen(body);
        rht[0].value = atoa(sz);
        rht[1].value = "text/html";
        return NULL;
    }

    char* p;
    if(r->args_start == NULL)
        p = dup_str(r->uri_start, r->uri_end);
    else { p = dup_str(r->uri_start, r->args_start-1); }



    char *path = malloc(strlen("/home/nelson/www/") + strlen(p));
    strcpy(path,"/home/nelson/www");

    if(r->uri_start == r->uri_end-1) {// means the uri is '/'
        append = "index.html";
        free(path);
        path = malloc(strlen("/home/nelson/www/") + strlen(append));
        strcpy(path,"/home/nelson/www");
        strcat(path, append);
    } else { strcat(path, p); }

    file_t* f_head = new_file_header(path);

    if(f_head == NULL){
        return "No File";
    }

    char* cont = mmap(NULL, f_head->info.st_size, PROT_READ, MAP_PRIVATE, f_head->fd, 0);
    stat(f_head->path, &f_head->info);
    size_t sz = f_head->info.st_size;
    if(sz > 4294967296)//4 GB MAX
        return NULL;
    sprintf(sz_str, "%ld", sz);
    rht_flag = 1;
    rht[0].value = strdup(sz_str);
    if(f_head->extention == jpg || f_head->extention == png || f_head->extention == mpeg || f_head->extention == ico){
        sprintf(cnt_type, "image/%s", f_head->ext_txt);
        rht[1].value = strdup(cnt_type);
    } else if(f_head->extention == txt || f_head->extention == html || f_head->extention == css) {
        if(f_head->extention == txt)
            sprintf(cnt_type, "text/%s", "plain");
        else
            sprintf(cnt_type, "text/%s", f_head->ext_txt);
        rht[1].value = strdup(cnt_type);
    } else {
        sprintf(cnt_type, "application/%s", f_head->ext_txt);
        rht[1].value = strdup(cnt_type);
    }

    return cont;
}

status parse_routine(request_t* r){
    /*Parse module for request msg
     *aligns the parse functions
     *
     * */
    rht[2].value = dt();
    rht[4].value = Server;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *ret, *body;

    int rls = parse_request_line(r);
    switch (rls){
        case HTTP_INVALID_METHOD:
            rht_flag = 1, sl_flag = 1;
            sl.code = "501" ,sl.reason="Not Implemented";
            body = "<html><h1>ERROR 501</h1><body>Invalid/ Unimplemented method</body></html>";
            body_gen(r, body);
            r->resp = write_msg(body);
            return 0;
        case HTTP_BAD_REQUEST:
            rht_flag = 1, sl_flag = 1;
            sl.code = "400" ,sl.reason="Bad Request";
            body = "<html><h1>ERROR 400</h1><body>Bad Request</body></html>";
            body_gen(r, body);
            r->resp = write_msg(body);
            return 0;
        case HTTP_INVALID_REQUEST:
            rht_flag = 1, sl_flag = 1;
            sl.code = "400" ,sl.reason="Bad Request";
            body = "<html><h1>ERROR 400</h1><body>Bad Request</body></html>";
            body_gen(r, body);
            r->resp = write_msg(body);
            return 0;
        case HTTP_REQ_LINE_OK:
            goto check_url;
        default:
            return -1;
    }

check_url:
    if(r->method == HTTP_GET){
        char* rsp = body_gen(r, NULL);
        if((strcmp(rsp, "No File")) == 0){
            rht_flag = 1, sl_flag = 1;
            sl.code = "404" ,sl.reason="Not Found";
            body = "<html><h1>ERROR 404</h1><body>Nt Found</body></html>";
            body_gen(r, body);
            r->resp = write_msg(body);
            return 0;
        }
    }

    if(r->has_query){
        rls = parse_args(r);
        switch(rls){
            case HTTP_NO_CATEGORY:
                rht_flag = 1, sl_flag = 1;
                sl.code = "400" ,sl.reason="Bad Request";
                body = "<html><h1>ERROR 400</h1><body>Bad Request</body></html>";
                body_gen(r, body);
                r->resp = write_msg(body);
                return 0;
            case HTTP_ARGS_OK:
                break;
            default:
                return -1;
        }
    }

    rls = parse_header_lines(r);
    switch(rls){
        case HTTP_BAD_REQUEST:
            rht_flag = 1, sl_flag = 1;
            sl.code = "400" ,sl.reason="Bad Request";
            body = "<html><h1>ERROR 400</h1><body>Bad Request</body></html>";
            body_gen(r, body);
            r->resp = write_msg(body);
            return 0;
        case HTTP_HEADERS_OK:
            break;
        default:
            return -1;
    }

    char* ka_f;
    if((ka_f = ht_get(r->Headers, "Connection")) != NULL){
        if(strcmp(ka_f, "keep-alive") == 0)
            r->ka = 1;
    }

    char* encoding;
    if(ht_get(r->Headers, "Content-Length") != NULL || (encoding = ht_get(r->Headers, "Transfer-Encoding")) != NULL){
        if(strcmp(encoding, "chunked") == 0){
            rls = parse_chunked(r);
            switch(rls) {
                case HTTP_NO_CATEGORY:
                    rht_flag = 1, sl_flag = 1;
                    sl.code = "400" ,sl.reason="Bad Request";
                    body = "<html><h1>ERROR 400</h1><body>Bad Request</body></html>";
                    body_gen(r, body);
                    r->resp = write_msg(body);
                    return 0;
                case HTTP_CLIENT_ERROR:
                    rht_flag = 1, sl_flag = 1;
                    sl.code = "500" ,sl.reason="Internal Server Error";
                    body = "<html><h1>ERROR 500</h1><body>Server Error</body></html>";
                    body_gen(r, body);
                    r->resp = write_msg(body);
                    return 0;
                case HTTP_CHUNKS_OK:
                    break;
                default:
                    return -1;
            }
        }
    }



    return 1;
}

status handler(char* raw, int sockfd){
    request_t* r = createRequest(sockfd);
    r->MSG = raw;
    char* body;

    int pr = parse_routine(r);
    switch(pr){
        case -1:
            rht_flag = 1, sl_flag = 1;
            sl.code = "500" ,sl.reason="Internal Server Error";
            body = "<html><h1>ERROR 500</h1><body>Server Error</body></html>";
            body_gen(r, body);
            r->resp = write_msg(body);
            HTTPMsgTransfer(r, SEND, r->resp);
            return 0;
        case 0:
            if(r->resp != NULL)
                HTTPMsgTransfer(r, SEND, r->resp);
            if(r->resp == NULL){
                rht_flag = 1, sl_flag = 1;
                sl.code = "500" ,sl.reason="Internal Server Error";
                body = "<html><h1>ERROR 500</h1><body>Server Error</body></html>";
                body_gen(r, body);
                r->resp = write_msg(body);
                HTTPMsgTransfer(r, SEND, r->resp);
                return 0;
            }
            break;
        case 1:
            break;
        default:
            return -1;
   }

   rht_flag = 1, sl_flag = 1;
    sl.code = "200", sl.reason = "OK";
    r->resp = write_msg(body_gen(r, NULL));
    HTTPMsgTransfer(r, SEND, r->resp);
    return 1;
}