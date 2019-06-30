//
// Created by nelson on 6/29/19.
//

#include "can_file.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>

/*Hash func for f_name_hash*/
static long int hash (char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + hash * 33 + c;

    return hash;
}

/*get file name from path*/
static char*  get_file_name(char* path) {
    struct stat s;
    if(stat(path, &s) == 0) {
        if (s.st_mode & S_IFREG)
            return basename(strdup(path));
        else return NULL;
    } else return NULL;
}

static char* get_dir_name(char* path) {
    struct stat s;
    if(stat(path, &s) == 0) {
        if (s.st_mode & S_IFDIR)
            return dirname(strdup(path));
        else return NULL;
    } else return NULL;
}


static ext get_ext(file_t* file) {
    char* f_name = strdup(file->fname);
    char* name = strtok(f_name, ".");
    char* exte = strtok(NULL, ".");
    if(strcmp(exte, "png") == 0) {
        return png;
    }

    if(strcmp(exte, "exe") == 0) {
        return exe;
    }

    if(strcmp(exte, "jpg") == 0) {
        return jpg;
    }

    if(strcmp(exte, "txt") == 0) {
        return txt;
    }

    if(strcmp(exte, "css") == 0) {
        return css;
    }

    if(strcmp(exte, "html") == 0) {
        return html;
    }

    if(strcmp(exte, "js") == 0) {
        return js;
    }

    if(strcmp(exte, "rs") == 0) {
        return rs;
    }

    if(strcmp(exte, "py") == 0) {
        return py;
    }

    if(strcmp(exte, "php") == 0) {
        return php;
    }

    if(strcmp(exte, "ts") == 0) {
        return ts;
    }

    if(strcmp(exte, "mpeg") == 0) {
        return mpeg;
    }
}

/*                  API IMPL
 * -----------------------------------------
 * */

file_t* create_file_header(char* path) {
    file_t* f_walk = (file_t*)malloc(sizeof(file_t));
    if(f_walk == NULL)
        return NULL;

    f_walk->path = path;

    f_walk->fname = get_file_name(path);
    f_walk->crc32 = crc(*f_walk);

    long int f_name_hash = hash(f_walk->fname);

    f_walk->fname_hash = (f_name_hash < -1) ? (f_name_hash *= -1): (f_name_hash);
    f_walk->dir = dirname(strdup(path));
    f_walk->valid = 0;
    f_walk->fd = 0;
    f_walk->extention = get_ext(f_walk);

    return f_walk;
}

dir_t* create_dir(char* path) {
    dir_t* dir = (dir_t*)malloc(sizeof(dir_t));
    if(dir == NULL)
        return NULL;
    dir->path = path;
    dir->name = basename(strdup(path));
    walk(NULL, dir);
    return dir;
}

void walk(char* path, dir_t* dir_walk) {
    dir_t* dir;
    if(path != NULL) {
        dir = create_dir(path);
    } else { dir = dir_walk; }

    struct dirent* entry;
    DIR* dir_l = opendir(dir->path);

    int dir_size = 0;
    while((entry = readdir(dir_l)) != NULL) {
        if(entry->d_type == DT_REG)
            dir_size++;
    }

    char* dir_entries[dir_size];
    dir->data = dir_entries;

    int loop_v = 0;
    while((entry = readdir(dir_l)) != NULL) {
        dir->data[loop_v] = entry->d_name;
        loop_v++;
    }
    closedir(dir_l);
}

/*make a copy of file at f_headers at path*/
file_t make_copy_file(file_t f_headers, char* path) {
    file_t* f_header_l= create_file_header(strcat(path, f_headers.fname));

    if(f_headers.valid) {
        if (f_headers.fd == 0)
            f_headers.fd = open(f_headers.path, O_RDWR, S_IRUSR | S_IWUSR);

        stat(f_headers.path, &f_headers.info);

        f_header_l->fd = open(f_header_l->path, O_RDWR, S_IRUSR | S_IWUSR);
        stat(f_header_l->path, &f_header_l->info);

        char* o_content = mmap(NULL, f_headers.info.st_size, PROT_READ, MAP_PRIVATE, f_headers.fd, 0);

        char* i_cont = mmap(NULL, f_header_l->info.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, f_header_l->fd, 0);

        memcpy(i_cont, o_content, f_headers.info.st_size);

        close(f_headers.fd);
        close(f_header_l->fd);
        munmap(o_content, f_headers.info.st_size);
        munmap(i_cont, f_header_l->info.st_size);
    }

    return *f_header_l;
}

int crc(file_t f_walk) {
    return 0;
}