/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server.cpp
 * Author: xiaoyiluo
 * 
 * Created on June 19, 2018, 10:42 AM
 */

#include "server.h"

server::server() {
}

int server::open_listenfd(string path) {
    // create socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("socket_fd %d \n", socket_fd);

    // address
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path.data());

    // bind
    int res = bind(socket_fd, (struct sockaddr*) &addr, sizeof (addr));
    printf("bind res: %d \n", res);

    res = listen(socket_fd, 5);
    printf("listen res: %d \n", res);

    return socket_fd;
}

server::server(const server& orig) {
}

server::~server() {
}

