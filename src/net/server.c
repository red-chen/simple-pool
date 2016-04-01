#include "net/server.h"
#include "net/acceptor.h"

#include <stdio.h>
#include <stdlib.h>

struct simpel_server_t {
    int port;
    SimpleIOHandler* handler;
    SimpleServerConfig config;
    int index;
    SimpleIOThread** threads;
};

static void simple_server_init_config(SimpleServerConfig* input, SimpleServerConfig* out);

static void simple_server_new_conn_cb(void* user_data, int conn_fd); 

SimpleServer* simple_server_create(int port, SimpleIOHandler* handler, SimpleServerConfig* config) {
    SimpleServer* self = malloc(sizeof(SimpleServer));
    self->port = port;
    self->handler = handler;
    simple_server_init_config(config, self->config);
    self->index = 0;
    self->threads = malloc(sizeof(SimpleIOThread*) * self->config.io_thread_count);
    for (int i = 0; i < self->config.io_thread_count; i++) {
        self->threads[i] = simple_io_thread_create();
        simple_io_thread_start(self->threads[i]);
    }
    return self;
}

void simple_server_destroy(SimpleServer* self) {
    // TODO
}

void simple_server_start(SimpleServer* self) {
    SimpleAcceptor* acceptor = simple_acceptor_create(
        self->port,
        simple_server_new_conn_cb // 注册一个新建连接的回调
    );
    simple_acceptor_start(acceptor);
}

void simple_server_stop(SimpleServer* self) {
    // TODO
}

void simple_server_init_config(SimpleServerConfig* input, SimpleServerConfig* out) {
    out->io_thread_count = input->io_thread_count;
}

void simple_server_new_conn_cb(void* user_data, int conn_fd) {
    SimpleServer* self = (SimpleServer*) user_data;
    
    // 从IO线程池中轮询选择一个IO线程
    // TODO 这里会有大量的连接，需要考虑使用内存池
    SimpleConnection* conn = simple_connection_create(self->threads[self->index], conn_fd, self->handler);
    simple_connection_establish(conn);
    if (++(self->index) >= self->config.io_thread_count ) {
        self->index = 0;
    }
}
