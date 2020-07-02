//
// Created by nelson on 6/29/20.
//

#include "canon_procp.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/wait.h>

#define PrW 0
#define PrR 1
#define ChR 0
#define ChW 1

ssize_t wr(proc_t* p, int m, int fl){
    /*all reads/ writes are 2 bytes*/
    if (m > 99)
        return 0;
    char mc[3];
    if(m<10) {
        mc[0] = m;
        mc[1] = '\0';
    } else {
        mc[0] = m/10 + '0';
        mc[1] = m%10 + '0';
    }
    ssize_t r = write(p->pipe[fl][WR], mc, 2);
    return r; /*still returns what write would*/
}

int rd(proc_t* p, int fl){
    /*all reads are 2 bytes*/
    char buf[3];
    ssize_t r = read(p->pipe[fl][RD], buf, 2);
    switch(r){
        case 1:
            read(p->pipe[fl][RD], &buf[1], 1);
            break;
        case -1:
            perror("read");
            return -1;
        default:
            break;
    }
    int rt = buf[0] - '0';
    if(buf[1] != '\0')
        rt = (rt * 10) + (buf[1] - '0');
    return rt;
}

void rwr(proc_t* p){
    int tm = 0, r;
    tm = p->idl;
    cwait:
    pthread_cond_wait(&p->c, &p->l);
    if(tm == p->idl){
        r = rd(p, ChR);
        if(r == DIE && p->or != 1)
            exit(DIE);
    } else {
        tm = p->idl;
        wr(p, tm, ChW);
    }
    goto cwait;
}

proc_t* new_proc(int l, procs_t* p) {
    proc_t* np = malloc(sizeof(proc_t));
    if(np == NULL)
        return NULL;

    np->l_fd = l;

    if(pipe(np->pipe[0]) == -1)
        perror("pipe");
    if(pipe(np->pipe[1]) == -1)
        perror("pipe");

    np->pool = p;
    np->dead = 0;

    pthread_mutex_init(&np->l, NULL);
    pthread_cond_init(&np->c, NULL);

    np->idl = 0;

    return np;
}

int ch_pr(proc_t* p, sock* c){
    //child process
    struct epoll_event evs[5], ev, ev_p;
    int nfds, ei /*epoll instance*/, pa, fl=0;
    socklen_t pl = sizeof(c->port);
    char buf[256];

    pthread_create(&(p->id), NULL, (void *)rwr, p);
    pthread_detach(p->id);

    ei = epoll_create1(0);
    /*listen socket*/
    ev.events = EPOLLIN;
    ev.data.fd = p->l_fd;
    if(epoll_ctl(ei, EPOLL_CTL_ADD, p->l_fd, &ev) < 0) {
        perror("epoll_ctl");
        wr(p, DIE, ChW);
        exit(1);
    }

    /*pipe event*/
    ev_p.events = EPOLLIN;
    ev_p.data.fd = p->pipe[ChR][RD];
    if(epoll_ctl(ei, EPOLL_CTL_ADD, p->pipe[ChR][RD], &ev_p) < 0) {
        perror("epoll_ctl");
        wr(p, DIE, ChW);
        exit(1);
    }

    wait_tr:
    nfds = epoll_wait(ei, evs, 5, 1);
    if(nfds == 0){
        p->idl++;
        pthread_cond_signal(&p->c);
        goto wait_tr;
    }
    if(nfds < 0){
        perror("epoll_wait");
        wr(p, DIE, ChW);
        exit(DIE);
    }
    int i = 0;
    for(;i<nfds;i++){
        if(p->l_fd == evs[i].data.fd){
            if((pa = accept(p->l_fd, (struct sockaddr *)&c->port, &pl)) < 0)
                perror("accept");
            int x = 0;
            x = recv(pa, buf, 256, MSG_PEEK);
            switch(x){
                case 0:
                    perror("recv");
                    fflush(stdout);
                    shutdown(pa, SHUT_RDWR);
                    continue;
                case -1:
                    perror("recv");
                    fflush(stdout);
                    shutdown(pa, SHUT_RDWR);
                    continue;
                default:
                    break;
            }
            /*Nothing should exit while a request is being handled*/
            handler(create_request(pa));
            /*TODO: Keep alive connections*/

            close(pa);
        }
        if(p->pipe[ChR][RD] == evs[i].data.fd){
            pthread_cond_signal(&p->c);
            fl = 1;
        }
    }
    if(!fl){
        p->idl = p->idl;
        wr(p, 0, ChW);
    }
    goto wait_tr;
}

void* clutch(void* p) {
    if(((proc_t*)p)->dead && !((proc_t*)p)->or){
        free(p);
        return NULL;
    }

    int r = rd((proc_t*)p, PrR);
    if(r < 0)
        return NULL;
    int ttl = (((proc_t*)p)->pool->idle * ((proc_t*)p)->pool->pool->job_queue->size);
    ttl += r;
    if(r > 20 && !((proc_t*)p)->or){
        wr(p, DIE, PrW);
        ttl -= ((proc_t*)p)->idl;
        ((proc_t*)p)->pool->idle = ttl/((proc_t*)p)->pool->pool->job_queue->size+1;
        free(p);
        return NULL;
    }
    if(r == DIE){
        free(p);
        return NULL;
    }
    ttl += r;
    int avg = ttl/((proc_t*)p)->pool->pool->job_queue->size;

    /*
     * between 0  - 5  ms of idle time, a process is created
     * between 6  - 15 ms nothing happens
     * above        15 ms a process is removed if any exist
     * */
    if(avg < 6){
        proc_t* pn = new_proc(((proc_t*)p)->pool->ch->sock_fd, ((proc_t*)p)->pool);
        enqueue(p, ((proc_t*)p)->pool->pool->job_queue);
        printf("forking on: %d by: %lu\n", pn->pid, pn->id);
        pid_t pid = fork();
        switch(pid){
            case -1:
                perror("fork");
                return NULL;
            case 0:
                pn->pid = getpid();
                enqueue(pn, pn->pool->pool->job_queue);
                int ret = ch_pr(pn, pn->pool->ch);
                exit(ret);
            default:
                ((proc_t*)p)->pool->idle = avg;
                enqueue(p, ((proc_t*)p)->pool->pool->job_queue);
                break;
        }
    }

    if(avg>15){
        enqueue(p, ((proc_t*)p)->pool->pool->job_queue);
        if(((proc_t*)p)->pool->pool->job_queue->size < 2)
            return NULL;
        proc_t* i = ((proc_t*)p)->pool->pool->job_queue->Head->r;
        node* c = ((proc_t*)p)->pool->pool->job_queue->Head;
        while(i->idl <= 15) {
            i = c->next->r;
            c = c->next;
        }
        if(i->idl>15){
            wr(p, DIE, PrW);
            ttl -= i->idl;
            i->pool->idle = ttl/i->pool->pool->job_queue->size+1;
            i->dead = 1;
            return NULL;
        }
    }
    return NULL;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int start_up(){
    procs_t* pl = malloc(sizeof(procs_t));
    pl->pool = pool_init(clutch, NULL);
    pl->idle = 0;
    pl->ch = conn_gen(8080);
    clock_t timer_s, timer_e, total;

    if(listen(pl->ch->sock_fd, 10)<0)
        perror("listen");

    proc_t* or = new_proc(pl->ch->sock_fd, pl);
    or->or = 1;
    pl->or = or;

    timer_s = clock();

    pid_t pt = fork();
    switch(pt){
        case -1:
            perror("fork");
            return 1;
        case 0:
            or->pid = getpid();
            enqueue(or, pl->pool->job_queue);
            int ret = ch_pr(or, pl->ch);
            return ret;
        default:
            break;
    }

    int lp = 1;
    while(lp){
        timer_e = clock();
        total = timer_e - timer_s;
        if((double)total/CLOCKS_PER_SEC > 2 && pl->or->id < 20){
            lp = 0;
            proc_t* pn = new_proc(pl->ch->sock_fd, pl);
            enqueue(pn, pl->pool->job_queue);
            printf("forking on: %d by: %lu\n", pn->pid, pn->id);
            pid_t pid = fork();
            switch(pid){
                case -1:
                    perror("fork");
                case 0:
                    pn->pid = getpid();
                    enqueue(pn, pn->pool->pool->job_queue);
                    int ret = ch_pr(pn, pn->pool->ch);
                    exit(ret);
                default:
                    enqueue(pn, pl->pool->job_queue);
                    break;
            }
        }
    }
    wait(NULL);
    return  1;
}
#pragma clang diagnostic pop
