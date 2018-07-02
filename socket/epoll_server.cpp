/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   epoll_server.cpp
 * Author: xiaoyiluo
 * 
 * Created on June 24, 2018, 7:27 PM
 */

#include "epoll_server.h"

epoll_server::epoll_server() {
}

void epoll_server::socket_bind(const char* ip, int port) {
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->server_fd < 0) {
        perror("create server socket error");
        return;
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof (sockaddr_in));
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    int ret = bind(this->server_fd, (sockaddr*) & serv_addr, sizeof (serv_addr));

    if (ret < 0) {
        perror("bind error");
        exit(1);
    }

    ret = listen(this->server_fd, LISTENQ);

    if (ret < 0) {
        perror("listen error");
        exit(1);
    }

    fprintf(stdout, "process %d start server on %s:%d success. \n", getpid(), ip, port);
}

void epoll_server::do_epoll() {
    //创建一个描述符
    int epoll_fd = epoll_create(FDSIZE);
    if (epoll_fd < 0) {
        perror("epoll create error");
        exit(1);
    }
    // 添加 sever_fd 的读事件
    add_event(epoll_fd, this->server_fd, EPOLLIN);
    epoll_event events[EPOLLEVENTS];
    char buf[MAXSIZE];
    while (1) {
        //获取已经准备好的描述符事件
        int ret = epoll_wait(epoll_fd, events, EPOLLEVENTS, -1);
        if (ret < 0) {
            perror("epoll_wait error");
            continue;
        }
        this->handle_events(epoll_fd, events, ret, this->server_fd, buf);
    }

    close(epoll_fd);
}

void epoll_server::add_event(int epollfd, int fd, int eve) {
    epoll_event event;
    event.events = eve;
    event.data.fd = fd;
    int res = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);

    if (res < 0) {
        fprintf(stderr, "Adding event[%d] of fd[%d] to epoll_fd[%d] error \n", eve, fd, epollfd);
        perror("event add error");
        exit(1);
    }

    fprintf(stdout, "Adding event[%d] of fd[%d] to epoll_fd[%d] success \n", eve, fd, epollfd);
}

void epoll_server::handle_events(int epollfd, epoll_event* events, int num, int listenfd, char* buf) {
    for (int i = 0; i < num; ++i) {
        int fd = events[i].data.fd;

        // 处理连接请求
        if (fd == listenfd) {
            if (events[i].events & EPOLLIN) {
                this->handle_accpet(epollfd, listenfd);
            }
        } else {
            // 读取连接的内容,如果存在多个客户端,用同一个 buf 是会有问题的.现在暂时先不考虑吧 TODO
            if (events[i].events & EPOLLIN) {
                memset(buf, 0, MAXSIZE);
                this->do_read(epollfd, fd, buf);
            }// 写数据回连接
            else if (events[i].events & EPOLLOUT) {
                this->do_write(epollfd, fd, buf);
            }
        }
    }
}

void epoll_server::handle_accpet(int epollfd, int listenfd) {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof (sockaddr_in);
    // 获取一个客户端连接
    int client_fd = accept(listenfd, (sockaddr*) & client_addr, &client_len);

    if (client_fd < 0) {
        fprintf(stderr, "accept client connection error, epoll_fd[%d] and sever_fd[%d] \n", epollfd, listenfd);
        perror("accept client connection error");
        exit(1);
    }

    printf("accept a new client: %s:%d\n", inet_ntoa(client_addr.sin_addr), (int) client_addr.sin_port);
    // 监听这个客户端的读事件
    this->add_event(epollfd, client_fd, EPOLLIN);
}

void epoll_server::do_read(int epollfd, int fd, char* buf) {
    // recieve data from client
    int count = recv(fd, buf, MAXSIZE, 0);
    if (count < 0) {
        perror("read error");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    } else if (count == 0) {
        fprintf(stdout, "client close.\n");
        delete_event(epollfd, fd, EPOLLIN);
        close(fd);
    } else {
        printf("recieve message[%s] from fd[%d] \n", buf, fd);
        //修改描述符对应的事件，由读改为写
        modify_event(epollfd, fd, EPOLLOUT);
    }
}

void epoll_server::do_write(int epollfd, int fd, char* buf) {
    // write the data to the client, 从客户端收到什么就写什么
    int nwrite = write(fd, buf, strlen(buf));
    if (nwrite == -1) {
        perror("write error:");
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
    } else
        modify_event(epollfd, fd, EPOLLIN);
}

void epoll_server::delete_event(int epollfd, int fd, int eve) {
    epoll_event event;
    event.events = eve;
    event.data.fd = fd;
    int res = epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);

    if (res < 0) {
        fprintf(stderr, "Deleting event[%d] of fd[%d] to epoll_fd[%d] error \n", eve, fd, epollfd);
        perror("event delete error");
        exit(1);
    }

    fprintf(stdout, "Deleting event[%d] of fd[%d] to epoll_fd[%d] success \n", eve, fd, epollfd);
}

void epoll_server::modify_event(int epollfd, int fd, int eve) {
    epoll_event event;
    event.events = eve;
    event.data.fd = fd;
    int res = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);

    if (res < 0) {
        fprintf(stderr, "Modifing event[%d] of fd[%d] to epoll_fd[%d] error \n", eve, fd, epollfd);
        perror("event modify error");
        exit(1);
    }

    fprintf(stdout, "Modifing event[%d] of fd[%d] to epoll_fd[%d] success \n", eve, fd, epollfd);
}

epoll_server::epoll_server(const epoll_server& orig) {
}

epoll_server::~epoll_server() {
}

