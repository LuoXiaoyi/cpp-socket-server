/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   epoll_server.h
 * Author: xiaoyiluo
 *
 * Created on June 24, 2018, 7:27 PM
 */

#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>

#define IPADDRESS   "127.0.0.1"
#define PORT        8080
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      1000
#define EPOLLEVENTS 100

class epoll_server {
public:
    epoll_server();
    epoll_server(const epoll_server& orig);
    virtual ~epoll_server();

    //创建套接字并进行绑定
    void socket_bind(const char* ip, int port);
    //IO多路复用epoll
    void do_epoll();
    //事件处理函数
    void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char *buf);
    //处理接收到的连接
    void handle_accpet(int epollfd, int listenfd);
    //读处理
    void do_read(int epollfd, int fd, char *buf);
    //写处理
    void do_write(int epollfd, int fd, char *buf);
    //添加事件
    void add_event(int epollfd, int fd, int state);
    //修改事件
    void modify_event(int epollfd, int fd, int state);
    //删除事件
    void delete_event(int epollfd, int fd, int state);

private:
    int server_fd;
};

#endif /* EPOLL_SERVER_H */

