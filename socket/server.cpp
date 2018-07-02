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
    if(path.length() > UNIX_PATH_MAX){
        cout << "the length of path: " << path << " is longer than " << UNIX_PATH_MAX << endl;
        return -1;
    }
    
    // create socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("socket_fd %d \n", socket_fd);

    // address
    struct sockaddr_un addr;
    memset(&addr,0,sizeof(sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path.data(),path.length());

    // bind
    int res = bind(socket_fd, (struct sockaddr*) &addr, sizeof (addr));
    if(res < 0) return -1;
    printf("bind res: %d \n", res);

    res = listen(socket_fd, 5);
    if(res < 0) return -1;
    printf("listen res: %d \n", res);

    return socket_fd;
}

server::server(const server& orig) {
}

server::~server() {
}

