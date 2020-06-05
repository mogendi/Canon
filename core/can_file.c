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
#include "crc32.h"

/*Hash func for f_name_hash*/
long int hash (char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++) != NULL)
        hash = ((hash << 5) + hash) + hash * 33 + c;

    return hash % 65536;
}

/*get file name from path*/
static char*  get_file_name(char* path) {
    struct stat s;
    if(stat(path, &s) == 0) {
        if (s.st_mode && S_IFREG)
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
    file->ext_txt = strtok(NULL, ".");
    if(strcmp(file->ext_txt, "png") == 0) {
        return png;
    }

    if(strcmp(file->ext_txt, "ico") == 0) {
        return ico;
    }

    if(strcmp(file->ext_txt, "jpg") == 0) {
        return jpg;
    }

    if(strcmp(file->ext_txt, "txt") == 0) {
        return txt;
    }

    if(strcmp(file->ext_txt, "css") == 0) {
        return css;
    }

    if(strcmp(file->ext_txt, "html") == 0) {
        return html;
    }

    if(strcmp(file->ext_txt, "js") == 0) {
        return js;
    }

    if(strcmp(file->ext_txt, "rs") == 0) {
        return rs;
    }

    if(strcmp(file->ext_txt, "py") == 0) {
        return py;
    }

    if(strcmp(file->ext_txt, "php") == 0) {
        return php;
    }

    if(strcmp(file->ext_txt, "ts") == 0) {
        return ts;
    }

    if(strcmp(file->ext_txt, "mpeg") == 0) {
        return mpeg;
    }
}

/*                  API IMPL
 * -----------------------------------------
 * */

file_t* new_file_header(char* path) {
    file_t* f_walk = (file_t*)malloc(sizeof(file_t));
    if(f_walk == NULL)
        return NULL;

    f_walk->path = path;

    f_walk->fname = get_file_name(path);

    f_walk->crc32 = crc(f_walk);

    long int f_name_hash = hash(f_walk->fname);

    f_walk->fname_hash = (f_name_hash < -1) ? (f_name_hash *= -1): (f_name_hash);
    f_walk->dir = dirname(strdup(path));
    f_walk->valid = 0;
    f_walk->fd = open(f_walk->path, O_RDONLY, S_IRUSR|S_IWUSR);
    f_walk->extention = get_ext(f_walk);

    return f_walk;
}

dir_t* new_dir(char* path) {
    dir_t* dir = (dir_t*)malloc(sizeof(dir_t));
    if(dir == NULL)
        return NULL;
    dir->path = path;
    dir->name = basename(strdup(path));
    dir->data = walk_path(path);
    return dir;
}

data_t* walk_path(char* path) {
    int size = 0;

    if(path == NULL){
        printf("NULL path\n");
        return NULL;
    }
    struct dirent* entry;
    DIR* dir_l = opendir(path);

    if(dir_l == NULL) {
        char** data = (char**)malloc(sizeof(char*) * 2);
        data[0] = path;
        data_t* ret = (data_t*)malloc(sizeof(data_t));
        ret->data = data;
        ret->size = 1;
        return ret;
    }

    while((entry = readdir(dir_l)) != NULL) {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        size++;
    }
    closedir(dir_l);

    char** data = (char**)malloc(sizeof(char*) * size + 1);
    int loop_v = 0;
    dir_l = opendir(path);

    while((entry = readdir(dir_l)) != NULL && loop_v < size) {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        data[loop_v] = strdup(entry->d_name);
        loop_v++;
    }

    closedir(dir_l);

    data_t* ret = (data_t*)malloc(sizeof(data_t));
    ret->size = (size_t)size;
    ret->data = data;
    return ret;
}

/*make a copy of file at f_headers at path*/
file_t make_copy_file(file_t f_headers, char* path) {
    file_t* f_header_l= new_file_header(strcat(path, f_headers.fname));

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
    }

    return *f_header_l;
}

u_int32_t crc(file_t* f_walk) {
    f_walk->fd = open(f_walk->path, O_RDWR, S_IRUSR);
    if(f_walk->fd == -1) {
        perror("open");
    }
    stat(f_walk->path, &f_walk->info);

    char* cont_temp = (char *)mmap(NULL, f_walk->info.st_size, PROT_READ, MAP_PRIVATE, f_walk->fd, 0);
    u_int32_t crc = crc32_text( cont_temp, f_walk->info.st_size );
    //f_walk->crc32 = crc;

    munmap(cont_temp, f_walk->info.st_size);
    close(f_walk->fd);
    return crc;
}

/*Assumes the path is a directory path*/
data_t* merge_path(char* path, char* path2) {
    data_t* path_data = walk_path(path);
    data_t* path2_data = walk_path(path2);
    if(path_data == NULL || path2_data == NULL) {
        printf("Merge failed, couldn't get path data\n");
        return NULL;
    }

    char** new_dir_info = (char**)malloc(sizeof(char*) * (path2_data->size + path_data->size) );
    if(new_dir_info == NULL)
        return NULL;

    int loop = 0;
    while(loop < path_data->size) {
        new_dir_info[loop] = path_data->data[loop];
        loop++;
    }
    loop = 0;
    while(loop < path2_data->size) {
        new_dir_info[loop + path_data->size] = path2_data->data[loop];
        loop++;
    }
    data_t* ret = (data_t*)malloc(sizeof(data_t));
    ret->size = (path_data->size + path2_data->size) - 1;
    ret->data = new_dir_info;
    free(path_data);
    free(path2_data);
    return ret;
}