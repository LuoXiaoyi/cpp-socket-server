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
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include <unistd.h> 

#define DEFAULT_PORT 8000  
#define MAXLINE 4096  

using namespace std;


void internet_socket_server(){
    int socket_fd, connect_fd;  
    struct sockaddr_in servaddr;  
    char buff[4096];  
    int n;  
    //初始化Socket  
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
    //初始化  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。  
    servaddr.sin_port = htons(DEFAULT_PORT);      //设置的端口为DEFAULT_PORT  
  
    //将本地地址绑定到所创建的套接字上  
    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){  
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
    //开始监听是否有客户端连接  
    if( listen(socket_fd, 10) == -1){  
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
    printf("======waiting for client's request======\n");  
    while(1){  
        //阻塞直到有客户端连接，不然多浪费CPU资源。  
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){  
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);  
            continue;  
        }  
        //接受客户端传过来的数据  
        n = recv(connect_fd, buff, MAXLINE, 0);  
        //向客户端发送回应数据  
        if(!fork()){
            if(send(connect_fd, "Hello,you are connected!\n", 26,0) == -1)  
                perror("send error");  
            close(connect_fd);  
            exit(0);  
        }  
        buff[n] = '\0';  
        printf("recv msg from client: %s\n", buff);  
        close(connect_fd);  
    }
    close(socket_fd); 
}


void file_socket_server(){
    string path = "/tmp/socket/socket.tmp";
    server s;

    struct sockaddr_un client_addr;
    int server_fd = s.open_listenfd(path);
    if (server_fd < 0) {
        cerr << "server created error." << endl;
        return;
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
}

/*
 * 
 */
int main(int argc, char** argv) {
    // file_socket_server();
    internet_socket_server();
    return 0;
}



