/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   select_server.cpp
 * Author: xiaoyiluo
 * 
 * Created on June 23, 2018, 9:27 PM
 */

#include "select_server.h"

int select_server::create_server_fd(const char* ip, const int port) {
    // 创建一个套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "create socket fail,erron:%d,reason:%s\n",
                errno, strerror(errno));
        return -1;
    }

    /*一个端口释放后会等待两分钟之后才能再被使用，SO_REUSEADDR是让端口释放后立即就可以被再次使用。*/
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)) < 0) {
        fprintf(stderr, "set reuse error, errno[%d], error msg[%s]",
                errno, strerror(errno));
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof (sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_aton(ip, &server_addr.sin_addr);

    int result = bind(server_fd, (sockaddr*) & server_addr, sizeof (sockaddr_in));
    if (result < 0) {
        fprintf(stderr, "bind socket to address[ip: %s, port: %u] error,erron:%d,reason:%s\n",
                ip, port, errno, strerror(errno));
        return -1;
    }

    listen(server_fd, LISTEN_Q);

    fprintf(stdout, "bind socket to address[ip: %s, port: %u] success\n",
            ip, port);
    return server_fd;
}

int select_server::server_init() {
    server_ctx = (server_context*) malloc(sizeof (server_context));
    if (server_ctx != NULL) {
        return -1;
    }

    memset(&server_ctx, 0, sizeof (server_ctx));

    int i = 0;
    while (i < SIZE) {
        server_ctx->client_fds[i] = -1;
        ++i;
    }

    return 0;
}

void select_server::handle_client_request() {
    int retval = 0;
    printf("fd_set size: %d ", (int) sizeof (fd_set));
    fd_set* p_read_fds = &server_ctx->all_client_fds;
    struct timeval time_out;

    while (1) {
        // 每次调用select前都要重新设置文件描述符和时间，因为事件发生后，文件描述符和时间都被内核修改啦
        FD_ZERO(p_read_fds);
        // 添加套接字
        FD_SET(server_fd, p_read_fds);
        server_ctx->max_fd = server_fd;

        time_out.tv_sec = 30; // 30 s
        time_out.tv_usec = 0; // 0 us

        // 添加客户端套接字
        int i = 0;
        while (i < server_ctx->client_cnt) {
            int cli_fd = server_ctx->client_fds[i];
            if (cli_fd != -1) {
                FD_SET(cli_fd, p_read_fds);
                server_ctx->max_fd = cli_fd > server_ctx->max_fd ?
                        cli_fd : server_ctx->max_fd;
            }
            i++;
        }
        printf("max_fd: %d, and the total number of client is %d \n", server_ctx->max_fd, server_ctx->client_cnt);
        //开始轮询接受处理服务端和客户端套接字
        retval = select(server_ctx->max_fd + 1,
                p_read_fds, NULL, NULL, &time_out);

        if (retval == -1) {
            fprintf(stderr, "select error:%s.\n", strerror(errno));
            return;
        }

        if (retval == 0) {
            fprintf(stdout, "select is timeout.\n");
            continue;
        }

        if (FD_ISSET(server_fd, p_read_fds)) {
            // 监听客户端请求
            accept_client_request();
        } else {
            // 接受处理客户端请求
            recv_client_request_msg(p_read_fds);
        }
    }
}

void select_server::accept_client_request() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof (client_addr);
    int client_fd = -1;

    printf("accpet client request is called, client_addr_len: %u .\n", client_addr_len);

    while (1) {
        client_fd = accept(server_fd, (sockaddr*) & client_addr, &client_addr_len);
        printf("waiting for 1000s \n");
        sleep(1000);
        if (client_fd == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                fprintf(stderr, "accept fail,error:%s\n", strerror(errno));
                return;
            }
        }

        fprintf(stdout, "accept a new client: %s:%d, client_fd: %d \n",
                inet_ntoa(client_addr.sin_addr), client_addr.sin_port, client_fd);

        // 将新的连接添加到客户端连接中
        if (server_ctx->client_cnt < SIZE) {
            server_ctx->client_fds[server_ctx->client_cnt] = client_fd;
            server_ctx->client_cnt++;
            printf("add client_fd: %d to the client_fds.\n", client_fd);
        } else {
            fprintf(stderr, "too many clients.\n");
            return;
        }

        break;
    }

    printf("accpet client request is called end");
}

void select_server::recv_client_request_msg(fd_set * read_fds) {
    int i = 0, n = 0;
    int client_fd = -1;
    char buf[MAX_LINE] = {0};
    printf("recv buf is called. \n");

    for (i = 0; i < server_ctx->client_cnt; ++i) {
        client_fd = server_ctx->client_fds[i];
        if (client_fd < 0) {
            continue;
        }

        //判断客户端是否有数据
        if (FD_ISSET(client_fd, read_fds)) {
            // 接受客户端发过来的数据
            n = read(client_fd, buf, MAX_LINE);
            if (n <= 0) {
                // 表示客户端已经关闭，或者数据已经读取完毕
                FD_CLR(client_fd, &server_ctx->all_client_fds);
                close(client_fd);
                server_ctx->client_fds[i] = -1;
                // 将后面的 fd 往前面移动
                if (i < server_ctx->client_cnt - 1) {
                    int j = i + 1;
                    while (j < server_ctx->client_cnt) {
                        server_ctx->client_fds[j - 1] = server_ctx->client_fds[j];
                        j++;
                    }
                    // 最后一个置为-1
                    server_ctx->client_fds[j - 1] = -1;
                }

                server_ctx->client_cnt--;
                printf("client_fd: %d is closed, and removed from client_fds.\n",client_fd);
                continue;
            }

            // 把从客户端接受到的数据重新写回给客户端
            printf("recv buf is :%s\n", buf);
            write(client_fd, buf, strlen(buf) + 1);
        }
    }



}

select_server::select_server(const select_server& orig) {
}

select_server::~select_server() {
    if (server_ctx != NULL) {
        free(server_ctx);
        server_ctx = NULL;
    }

    if (this->server_fd != -1) {
        close(this->server_fd);
    }
}

