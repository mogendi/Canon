//
// Created by nelson on 6/29/20.
//

#include "canon_procp.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#define STARVED 3

proc_t* new_proc(int l, procs_t* p) {
    proc_t* np = malloc(sizeof(proc_t));
    if(np == NULL)
        return NULL;

    np->l_fd = l;

    np->pool = p;
    np->dead = 0;

    np->idl = 0;

    np->or = 0;

    return np;
}

int ch_pr(proc_t* p){
    //child process
    struct epoll_event evs[2], ev;
    int nfds, ei /*epoll instance*/, pa, fl=0, ttl_rs = 0;
    socklen_t pl = sizeof(p->pool->ch);
    char buf[256];

    ei = epoll_create1(0);
    /*listen socket*/
    ev.events = EPOLLIN;
    ev.data.fd = p->l_fd;
    if(epoll_ctl(ei, EPOLL_CTL_ADD, p->l_fd, &ev) < 0) {
        perror("epoll_ctl");
        if(p->or)
            return 1;
        else{
            exit(1);
        }
    }

    wait_tr:
    nfds = epoll_wait(ei, evs, 5, 1);
    if(nfds == 0){
        p->idl++;
        if(p->idl > 30){
            p->pool->wrk_ttl -= ttl_rs;
            if(p->or)
                goto wait_tr;
            exit(STARVED);
        }
        goto wait_tr;
    }
    if(nfds < 0){
        perror("epoll_wait");
        return -1;
    }
    /*There's only 1 fd im waiting on so no loop is necessary*/
    if((pa = accept(p->l_fd, (struct sockaddr *)&p->pool->ch->port, &pl)) < 0)
        perror("accept");
    int x = 0;
    x = recv(pa, buf, 256, MSG_PEEK);
    switch(x){
        case 0:
            perror("recv");
            fflush(stdout);
            shutdown(pa, SHUT_RDWR);
            goto wait_tr;
        case -1:
            perror("recv");
            fflush(stdout);
            shutdown(pa, SHUT_RDWR);
            goto wait_tr;
        default:
            break;
    }
    handler(create_request(pa));
    fl++;
    /*TODO: Keep alive connections*/
    shutdown(pa, SHUT_RDWR);
    close(pa);

    if(fl > 19){
        ttl_rs += fl;
        fl = 0;
        kill(p->pool->mpid, SIGUSR1);
    }

    goto wait_tr;
}

int start_up(){
    signal(SIGCHLD, SIG_IGN);

    procs_t* mp = malloc(sizeof(procs_t));
    mp->q = new_q();
    mp->mpid = getpid();
    mp->idl = 0; mp->wrk_ttl = 0;
    int i;

    mp->ch = conn_gen(8080);

    if(listen(mp->ch->sock_fd, 10)<0){
        perror("listen");
        exit(-1);
    }

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    proc_t *or = new_proc(mp->ch->sock_fd, mp), *np;
    mp->or = or;
    or->or = 1;
    enqueue(or, mp->q);

    pid_t or_f = fork();
    switch(or_f){
        case 0:
            or->pid = getpid();
            while(1)
                return ch_pr(or);
        case -1:
            perror("fork");
            exit(-1);
        default:
            printf("main: %d\n", getpid());
            break;
    }

    int k = 0;
    int SGW;
    idle:
    i = mp->wrk_ttl;
    SGW = sigwait(&sigset, &k);
    if(SGW == 0 && k == SIGUSR1) {
        mp->wrk_ttl++;
        if (i == mp->wrk_ttl)
            goto idle;
        else {
            mp->idl = mp->wrk_ttl / mp->q->size;
            int j = mp->idl - 1;
            while (j) {
                printf("Idl avg: %d\n", j);
                np = new_proc(mp->ch->sock_fd, mp);
                enqueue(np, mp->q);
                j--;
                or_f = fork();
                switch (or_f) {
                    case 0:
                        np->pid = getpid();
                        int l = ch_pr(np);
                        exit(l);
                    case -1:
                        perror("fork");
                        np->dead = 1;
                        goto idle;
                    default:
                        break;
                }
            }
            goto idle;
        }
    } else{
        perror("sigwait");
    } goto idle;
}
