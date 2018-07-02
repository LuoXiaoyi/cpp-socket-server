/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   poll_server.h
 * Author: xiaoyiluo
 *
 * Created on June 24, 2018, 4:49 PM
 */

#ifndef POLL_SERVER_H
#define POLL_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define IPADDRESS   "127.0.0.1"
#define PORT        8080
#define MAXLINE     1024
#define LISTENQ     5
#define OPEN_MAX    1000
#define INFTIM      -1

class poll_server {
public:
    poll_server();
    poll_server(const poll_server& orig);
    virtual ~poll_server();

    //创建套接字并进行绑定
    int socket_bind(const char* ip, int port);
    //IO多路复用poll
    void do_poll(int listenfd);
    //处理多个连接
private:
        void handle_connection(struct pollfd *connfds, int num);

};

#endif /* POLL_SERVER_H */

