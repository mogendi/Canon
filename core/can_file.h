//
// Created by nelson on 6/29/19.
//

#ifndef CANON_CAN_FILE_H
#define CANON_CAN_FILE_H

#include <sys/stat.h>
#include <unitypes.h>
#include <zconf.h>

typedef struct file file_t;

typedef struct dir_s dir_t;

typedef struct stat stat_t;

typedef struct data_p data_t;

typedef enum {
    jpg,
    png,
    exe,
    mpeg,

    txt,
    html,
    css,
    js,
    py,
    php,
    ts,
    rs,
} ext;

struct file {
    int fd;

    char* path;

    char* dir;

    char* fname;
    int fname_hash;
    stat_t info;
    ext extention;

    int valid;
    u_int32_t crc32;
};

struct dir_s {
    char* path;

    char* name;

    data_t* data;

    int size;
};

/*Its necessary to retain the size info with the data*/
struct data_p {
    char** data;
    size_t size;
};


file_t* new_file_header(char* path);

dir_t* new_dir(char* path);

u_int32_t crc(file_t* f_walk);

file_t make_copy_file(file_t f_headers, char* path);

/*assumes path is a dir*/
void walk_dir(dir_t* dir);

data_t* walk_path(char* path);

data_t* merge_path(char* path, char* path2);

long int hash (char* str);

#endif //CANON_CAN_FILE_H
