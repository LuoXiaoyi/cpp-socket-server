/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server.h
 * Author: xiaoyiluo
 *
 * Created on June 19, 2018, 10:42 AM
 */

#ifndef SERVER_H
#define SERVER_H

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX   sizeof(((struct sockaddr_un *)0)->sun_path)
#endif

#include <netinet/in.h>    // for sockaddr_in  
#include <sys/types.h>    // for socket  
#include <sys/socket.h>    // for socket  
#include <stdio.h>        // for printf  
#include <stdlib.h>        // for exit  
#include <string.h>        // for bzero 
#include <sys/un.h>
#include <cstring>
#include <string>
#include <errno.h>

using namespace std;


class server {
public:
    server();
    server(const server& orig);
    virtual ~server();
    int open_listenfd(string path);
private:

};

#endif /* SERVER_H */

