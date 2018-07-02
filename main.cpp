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
#include <arpa/inet.h>
#include "socket/select_server.h"
#include "socket/poll_server.h"
#include "socket/epoll_server.h"

#define DEFAULT_PORT 8000  
#ifndef MAXLINE
#define MAXLINE 4096 
#endif

#define IP_ADDR "127.0.0.1"
#ifndef PORT
#define PORT 8080
#endif
using namespace std;

void internet_socket_server();
void file_socket_server();
void multi_process_test();
void test_select_server();
void test_poll_server();
void test_epoll_server();

/*
 * 
 */
int main(int argc, char** argv) {
    // multi_process_test();
    // file_socket_server();
    // internet_socket_server();
    // test_select_server();
    // test_poll_server();
    test_epoll_server();
    return 0;
}

void test_epoll_server() {
    epoll_server p_server;
    p_server.socket_bind(IP_ADDR, PORT);
    p_server.do_epoll();
}

void test_poll_server() {
    poll_server p_server;
    int socket_fd = p_server.socket_bind(IP_ADDR, PORT);
    p_server.do_poll(socket_fd);
}

void test_select_server() {
    select_server s_server;
    s_server.create_server_fd(IP_ADDR, PORT);
    s_server.handle_client_request();
}

void file_socket_server() {
    string path = "/tmp/socket/socket.tmp";
    server s;

    struct sockaddr_un client_addr;
    bzero(&client_addr, sizeof (sockaddr_un));
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
        cout << "client sun_family: " << client_addr.sun_family << endl;
        if (strlen(client_addr.sun_path) > 0)
            printf(" client path: %s", client_addr.sun_path);

        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        ssize_t count = recv(conn_fd, buffer, BUFFER_SIZE, 0);

        if (count < 0) {
            cout << "Server Recieve Data Failed!" << endl;
            break;
        }

        cout << "receive count: " << count << endl;

        string str;
        str.append(buffer, count);
        cout << "receive cmd: " << str << endl;

        str = "server says: glad to see you";
        send(conn_fd, str.data(), strlen(str.data()), 0);
    }

    close(server_fd);
}

void multi_process_test() {
    // file_socket_server();
    // internet_socket_server();
    pid_t pid;
    int count = 0;
    pid = fork(); //fork一个进程,这样,接下来的代码会同时被两个进程都执行
    if (pid == 0) { //pid为0,
        printf("this is child process, pid is %d\n", getpid());
        count += 2;
        printf("count = %d\n", count);
    } else if (pid > 0) {
        printf("this is father process, pid is %d\n", getpid());
        count++;
        printf("count = %d\n", count);
    } else {
        fprintf(stderr, "ERROR:fork() failed!\n");
    }
    cout << "hello pid : " << getpid() << endl;
    sleep(1000); //新加入的行，让程序在这里暂停10秒，父进程和子进程都会执行这行代码
}

void internet_socket_server() {
    int socket_fd, connect_fd;
    struct sockaddr_in servaddr;
    char buff[4096];
    int n;
    //初始化Socket  
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    //初始化  
    memset(&servaddr, 0, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。  
    servaddr.sin_port = htons(DEFAULT_PORT); //设置的端口为DEFAULT_PORT  

    //将本地地址绑定到所创建的套接字上  
    if (bind(socket_fd, (struct sockaddr*) &servaddr, sizeof (servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    printf("server started %s, on port %d \n", inet_ntoa(servaddr.sin_addr), (int) ntohs(servaddr.sin_port));

    //开始监听是否有客户端连接  
    if (listen(socket_fd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    printf("======waiting for client's request======\n");
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof (sockaddr_in));
    socklen_t len;
    while (1) {
        //阻塞直到有客户端连接，不然多浪费CPU资源。  
        if ((connect_fd = accept(socket_fd, (struct sockaddr*) &client_addr, &len)) == -1) {
            printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
            continue;
        }

        printf("client from %s, and port is %d \n", inet_ntoa(client_addr.sin_addr), (int) ntohs(client_addr.sin_port));
        //接受客户端传过来的数据  
        n = recv(connect_fd, buff, MAXLINE, 0);
        //向客户端发送回应数据  
        if (!fork()) {
            cout << "fork --> " << getpid() << endl;
            if (send(connect_fd, "Hello,you are connected!\n", 26, 0) == -1)
                perror("send error");
            close(connect_fd);
            exit(0);
        }
        buff[n] = '\0';
        printf("%d recv msg from client: %s\n", getpid(), buff);
        close(connect_fd);
    }
    close(socket_fd);
}

