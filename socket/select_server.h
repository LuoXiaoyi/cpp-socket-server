/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   select_server.h
 * Author: xiaoyiluo
 *
 * Created on June 23, 2018, 9:27 PM
 */

#ifndef SELECT_SERVER_H
#define SELECT_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#define MAX_LINE 1024
#define LISTEN_Q 1
#define SIZE 10

struct server_context {
    int client_cnt; // 客户端个数
    int client_fds[SIZE]; // 客户端句柄
    fd_set all_client_fds; // 客户端句柄集合
    int max_fd; // 句柄最大值
};

class select_server {
public:

    select_server() : server_ctx(NULL),server_fd(-1) {
	server_init();
    }
    select_server(const select_server& orig);
    virtual ~select_server();
    
    int create_server_fd(const char* ip, const int port);
    int server_init();
    void handle_client_request();
private:   
    void accept_client_request();
    void recv_client_request_msg(fd_set* read_fds); 
private:
    server_context * server_ctx;
    int server_fd;
};

#endif /* SELECT_SERVER_H */

