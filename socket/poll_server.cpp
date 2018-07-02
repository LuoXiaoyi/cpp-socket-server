/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   poll_server.cpp
 * Author: xiaoyiluo
 * 
 * Created on June 24, 2018, 4:49 PM
 */

#include "poll_server.h"

poll_server::poll_server() {
}

void poll_server::do_poll(int listenfd){
    int  connfd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    struct pollfd clientfds[OPEN_MAX];
    int maxi;
    int i;
    int nready;
    //添加监听描述符
    clientfds[0].fd = listenfd;
    clientfds[0].events = POLLIN;
    //初始化客户连接描述符
    for (i = 1;i < OPEN_MAX;i++)
        clientfds[i].fd = -1;
    maxi = 0;
    //循环处理
    for ( ; ; )
    {
        //获取可用描述符的个数
        nready = poll(clientfds,maxi+1,INFTIM);
        if (nready == -1)
        {
            perror("poll error:");
            exit(1);
        }
        //测试监听描述符是否准备好
        if (clientfds[0].revents & POLLIN)
        {
            cliaddrlen = sizeof(cliaddr);
            //接受新的连接
            if ((connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen)) == -1)
            {
                if (errno == EINTR)
                    continue;
                else
                {
                   perror("accept error:");
                   exit(1);
                }
            }
            fprintf(stdout,"accept a new client: %s:%d \n", inet_ntoa(cliaddr.sin_addr),(int)cliaddr.sin_port);
            //将新的连接描述符添加到数组中
            for (i = 1;i < OPEN_MAX;i++)
            {
                if (clientfds[i].fd < 0)
                {
                    clientfds[i].fd = connfd;
                    break;
                }
            }
            if (i == OPEN_MAX)
            {
                fprintf(stderr,"too many clients.\n");
                exit(1);
            }
            //将新的描述符添加到读描述符集合中
            clientfds[i].events = POLLIN;
            //记录客户连接套接字的个数
            maxi = (i > maxi ? i : maxi);
            if (--nready <= 0)
                continue;
        }
        //处理客户连接
        handle_connection(clientfds,maxi);
    }
}

void poll_server::handle_connection(pollfd* connfds, int num){
    int i,n;
    char buf[MAXLINE];
    memset(buf,0,MAXLINE);
    for (i = 1;i <= num;i++)
    {
        if (connfds[i].fd < 0)
            continue;
        //测试客户描述符是否准备好
        if (connfds[i].revents & POLLIN)
        {
            //接收客户端发送的信息
            n = read(connfds[i].fd,buf,MAXLINE);
            if (n == 0)
            {
                close(connfds[i].fd);
                connfds[i].fd = -1;
                continue;
            }
           // printf("read msg is: ");
            write(STDOUT_FILENO,buf,n);
            //向客户端发送buf
            write(connfds[i].fd,buf,n);
        }
    }
}

int poll_server::socket_bind(const char* ip, int port){
    int  listenfd;
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (listenfd == -1)
    {
        perror("socket error:");
        exit(1);
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)
    {
        perror("bind error: ");
        exit(1);
    }
    
    listen(listenfd,LISTENQ);
    
    printf("%d server started on: %s:%d \n", getpid(),ip,port);
    return listenfd;
}

poll_server::poll_server(const poll_server& orig) {
}

poll_server::~poll_server() {
}

