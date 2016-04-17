#include "net/server.h"

#include "net/acceptor.h"
#include "net/connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct simple_server_t {
    int port;
    int index;
    SimpleHandler* handler;
    SimpleServerConfig config;
    SimpleIOThread** threads;
};

static void simple_server_init_config(SimpleServerConfig* input, SimpleServerConfig* out);

static void simple_server_new_conn_cb(void* user_data, int conn_fd); 

SimpleServer* simple_server_create(int port, SimpleHandler* handler, SimpleServerConfig* config) {
    SimpleServer* self = malloc(sizeof(SimpleServer));
    self->port = port;
    self->handler = handler;
    simple_server_init_config(config, &(self->config));
    self->index = 0;
    self->threads = malloc(sizeof(SimpleIOThread*) * self->config.io_thread_count);
    for (int i = 0; i < self->config.io_thread_count; i++) {
        char name[64] = {0};
        sprintf(name, "server_t_%d", i);
        self->threads[i] = simple_io_thread_create(name);
    }
    return self;
}

void simple_server_destroy(SimpleServer* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_destroy(self->threads[i]);
    }
    free(self->threads);
    free(self);
}

void simple_server_start(SimpleServer* self) {
    // TODO 这里是使用的工作的线程，如果工作的线程被挂起，那么整个接收
    // 连接也将被挂起，需要考虑优化。
    SimpleAcceptor* acceptor = simple_acceptor_create(
        self->port,
        simple_server_new_conn_cb, // 注册一个新建连接的回调
        self,
        self->threads[0]
    );
    simple_acceptor_start(acceptor);

    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_start(self->threads[i]);
    }
}

void simple_server_stop(SimpleServer* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_stop(self->threads[i]);
    }
}

void simple_server_wait(SimpleServer* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_join(self->threads[i]);
    }
}

void simple_server_init_config(SimpleServerConfig* input, SimpleServerConfig* out) {
    out->io_thread_count = input->io_thread_count;
    out->io_thread_count = out->io_thread_count <= 0 ? 1 : out->io_thread_count;
}

void simple_server_new_conn_cb(void* user_data, int conn_fd) {
    SimpleServer* self = (SimpleServer*) user_data;
    
    // 从IO线程池中轮询选择一个IO线程
    // TODO 这里会有大量的连接，需要考虑使用内存池
    // TODO 检查连接数，如果过多考虑拒绝
    SimpleConnection* conn = simple_connection_create(
            self->threads[self->index], 
            conn_fd, 
            self->handler);
    simple_connection_establish(conn);

    if (++(self->index) >= self->config.io_thread_count ) {
        self->index = 0;
    }
}
