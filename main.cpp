/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: xiaoyiluo
 *
 * Created on June 19, 2018, 9:57 AM
 */
#define BUFFER_SIZE 1024
#include <iostream>
#include "socket/server.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    string path = "/tmp/socket/socket.tmp";
    server s;

    struct sockaddr_un client_addr;
    int server_fd = s.open_listenfd(path);
    if (server_fd < 0) {
        cerr << "server created error." << endl;
        return -1;
    }
    cout << "server_fd: " << server_fd << endl;

    socklen_t clinet_len;
    while (1) {
        int conn_fd = accept(server_fd, (struct sockaddr*) &client_addr, &clinet_len);

        cout << "conn_fd: " << conn_fd << endl;

        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        ssize_t count = recv(conn_fd, buffer, BUFFER_SIZE, 0);

        if (count < 0) {
            cout << "Server Recieve Data Failed!" << endl;
            break;
        }
        
        cout << "receive count: " << count << endl;
        
        string str;
        str.append(buffer,count);
        cout << "receive cmd: " << str << endl;
        
        str = "server says: glad to see you";
        send(conn_fd, str.data(), strlen(str.data()), 0);
    }
    return 0;
}

