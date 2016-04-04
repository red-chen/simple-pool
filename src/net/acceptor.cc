#include "net/acceptor.h"

#include "io/io_thread.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_LISTENFD 1024

struct simple_acceptor_t {
    int port;
    int listen_fd;
    SimpleNewConnection* new_conn;
    // Acceptor 单独开一个线程，用于监听端口的新建连接事件
    SimpleIOThread* thread; 
};

// 监听端口可读事件回调
// 这个函数内部会调用Server的NewConnection函数
static void simple_acceptor_handle_read(EventLoop* loop, int sock, void* user_data, int mask);

static void simpel_acceptor_bind_and_listen(SimpleAcceptor* self);

SimpleAcceptor* simple_acceptor_create(int port, SimpleNewConnection* new_conn, SimpleIOThread* thread) {
    SimpleAcceptor* self = malloc(sizeof(SimpleAcceptor));
    self->port = port;
    self->new_conn = new_conn;
    self->thread = thread;
    return self;
}

void simple_acceptor_destroy(SimpleAcceptor* self) {
    // TODO
}

void simple_acceptor_start(SimpleAcceptor* self) {
    simpel_acceptor_bind_and_listen(self);

    simple_io_thread_add_file_event(
        self->thread,
        self->listen_fd,
        AE_READABLE,
        simple_acceptor_handle_read,
        self
    );
}

void simple_acceptor_stop(SimpleAcceptor* self) {
    // TODO
}

void simple_acceptor_wait(SimpleAcceptor* self) {
    // TODO
}

void simpel_acceptor_bind_and_listen(SimpleAcceptor* self) {
    bool on = true;
    self->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(self->listen_fd, F_SETFL, O_NONBLOCK); //no-block IO
    setsockopt(self->listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(self->port);

    if (-1 == bind(self->listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr))) {
        ASSERT(false, "bind error, error msg: %s.", strerror(errno));
    }

    if (-1 == listen(self->listen_fd, MAX_LISTENFD)) {
        ASSERT(false, "bind error, errno: %d.", errno);
    }
}

void simple_acceptor_handle_read(EventLoop* loop, int sock, void* user_data, int mask) {
    SimpleAcceptor* self = (SimpleAcceptor*) user_data;
    struct sockaddr_in client_addr;
    socklen_t sockaddr_len = sizeof(struct sockaddr_in);

    int conn_fd = accept(self->listen_fd, (struct sockaddr*) &client_addr, (socklen_t*) &sockaddr_len);
    if (conn_fd <= 0) {
        ASSERT(false, "accept error, conn_fd:%d, errno:%d ", conn_fd, errno);
    } else {
        fcntl(conn_fd, F_SETFL, O_NONBLOCK); //no-block IO
    }
    self->new_conn(self->user_data, conn_fd);
}