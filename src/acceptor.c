#include "acceptor.h"
#include "address.h"

#include "lemon/io_thread.h"
#include "lemon/assert.h"

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

struct SimpleAcceptor {
    SimpleAddress* addr;
    int listen_fd;
    SimpleNewConnection* new_conn;
    // Acceptor 单独开一个线程，用于监听端口的新建连接事件
    SimpleIOThread* thread; 
    void* user_data;
};

// 监听端口可读事件回调
// 这个函数内部会调用Server的NewConnection函数
static int simple_acceptor_handle_read(EventLoop* loop, int sock, void* user_data, int mask);

static void simple_acceptor_bind_and_listen(SimpleAcceptor* self);

SimpleAcceptor* simple_acceptor_create(
        SimpleAddress* addr,
        SimpleNewConnection* new_conn, 
        void* user_data, 
        SimpleIOThread* thread) {
    SimpleAcceptor* self = malloc(sizeof(SimpleAcceptor));
    self->addr = addr;
    self->new_conn = new_conn;
    self->thread = thread;
    self->user_data = user_data;
    return self;
}

void simple_acceptor_destroy(SimpleAcceptor* self) {
    free(self);
}

void simple_acceptor_start(SimpleAcceptor* self) {
    simple_acceptor_bind_and_listen(self);

    simple_io_thread_add_file_event(
        self->thread,
        self->listen_fd,
        AE_READABLE,
        simple_acceptor_handle_read,
        self
    );
}

void simple_acceptor_stop(SimpleAcceptor* self) {
    // 删除监听事件
    simple_io_thread_del_file_event(
        self->thread,
        self->listen_fd,
        AE_READABLE
    );
}

void simple_acceptor_wait(SimpleAcceptor* self) {
    // do nothing
}

void simple_acceptor_bind_and_listen(SimpleAcceptor* self) {
    bool on = true;

    if (AF_INET == simple_address_get_family(self->addr)) {
        self->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(simple_address_get_port(self->addr));

        fcntl(self->listen_fd, F_SETFL, O_NONBLOCK); //no-block IO
        setsockopt(self->listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        
        if (-1 == bind(self->listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr))) {
            ASSERT(false, "bind error, error msg: %s.", strerror(errno));
        }
    } else if (AF_INET6 == simple_address_get_family(self->addr)) {
        ASSERT(false, "not implement.");
    } else if (AF_UNIX == simple_address_get_family(self->addr)) {
        ASSERT(false, "not implement.");
    } else {
        ASSERT(false, "not support");
    }

    if (-1 == listen(self->listen_fd, MAX_LISTENFD)) {
        ASSERT(false, "bind error, errno: %d.", errno);
    }
}

int simple_acceptor_handle_read(EventLoop* loop, int sock, void* user_data, int mask) {
    SimpleAcceptor* self = (SimpleAcceptor*) user_data;

    int conn_fd = 0;
    SimpleAddress* client = NULL;
    SimpleAddress* server = NULL;
    if (AF_INET == simple_address_get_family(self->addr)) {
        struct sockaddr_in client_addr, server_addr;
        socklen_t addr_len = sizeof(struct sockaddr_in);
        
        conn_fd = accept(self->listen_fd, 
                (struct sockaddr*) &client_addr, 
                &addr_len);

        getsockname(conn_fd, (struct sockaddr*)&server_addr, &addr_len); 

        client = simple_address_create_inet(
                inet_ntoa(client_addr.sin_addr), // 处理内存释放的问题
                ntohs(client_addr.sin_port));

        server = simple_address_create_inet(
                inet_ntoa(server_addr.sin_addr), // 处理内存释放的问题
                ntohs(server_addr.sin_port));

    } else if (AF_INET6 == simple_address_get_family(self->addr)) {
        ASSERT(false, "not implement.");
    } else if (AF_UNIX == simple_address_get_family(self->addr)) {
        ASSERT(false, "not implement.");
    } else {
        ASSERT(false, "not support");
    }

    if (conn_fd <= 0) {
        ASSERT(false, "accept error, conn_fd:%d, errno:%d ", conn_fd, errno);
    } else {
        fcntl(conn_fd, F_SETFL, O_NONBLOCK); //no-block IO
    }

    self->new_conn(self->user_data, conn_fd, client, server);
    return AE_OK;
}
