#include "client.h"

#include "address.h"
#include "connection.h"

#include "simple/assert.h"

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
#include <signal.h>

struct SimpleClient {
    int index;
    SimpleAddress* addr;
    SimpleHandler* handler;
    SimpleClientConfig config;
    SimpleIOThread** threads;
};

static void simple_client_init_config(SimpleClientConfig* input, SimpleClientConfig* out);

SimpleClient* simple_client_create(
        SimpleAddress* addr,
        SimpleHandler* handler, 
        SimpleClientConfig* config) {
    SimpleClient* self = malloc(sizeof(SimpleClient));
    self->addr = addr;
    self->handler = handler;
    simple_client_init_config(config, &(self->config));
    self->index = 0;
    self->threads = malloc(sizeof(SimpleIOThread*) * self->config.io_thread_count);
    for (int i = 0; i < self->config.io_thread_count; i++) {
        char name[64] = {0};
        sprintf(name, "client_t_%d", i);
        self->threads[i] = simple_io_thread_create(name);
    }
    return self;
}

void simple_client_destroy(SimpleClient* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_destroy(self->threads[i]);
    }
    free(self->threads);
    free(self);
}

// 创建一个连接，并为连接绑定读写事件
int simple_client_connect(SimpleClient* self) {
    int conn_fd = 0;
    SimpleAddress* client = NULL;
    SimpleAddress* server = NULL;
    if (AF_INET == simple_address_get_family(self->addr)) {
        struct sockaddr_in server_addr, client_addr;
        socklen_t addr_len = sizeof(server_addr);
        bzero(&server_addr, sizeof(server_addr));

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(simple_address_get_addr(self->addr));
        server_addr.sin_port = htons(simple_address_get_port(self->addr));

        conn_fd = socket(AF_INET, SOCK_STREAM, 0);
        
        int ret = connect(
                conn_fd, 
                (struct sockaddr*)&server_addr, 
                addr_len);

        ASSERT(ret == 0, "connect the remote address fail !");
  
        getsockname(conn_fd, (struct sockaddr*)&client_addr, &addr_len); 

        client = simple_address_create_inet(
                inet_ntoa(client_addr.sin_addr), // TODO 处理内存释放的问题
                ntohs(client_addr.sin_port));

        server = simple_address_create_inet(
                inet_ntoa(server_addr.sin_addr), // TODO 处理内存释放的问题
                ntohs(server_addr.sin_port));

    } else if (AF_INET6 == simple_address_get_family(self->addr)) {
        ASSERT(false, "not implement");
    } else if (AF_UNIX == simple_address_get_family(self->addr)) {
        ASSERT(false, "not implement");
    } else {
        ASSERT(false, "not support");
    }

    SimpleConnection* conn = simple_connection_create(
            self->threads[self->index], 
            conn_fd, 
            self->handler, 
            client,
            server);

    // 绑定读写
    simple_connection_establish(conn);

    if (++(self->index) >= self->config.io_thread_count ) {
        self->index = 0;
    }

    return 0;
}

void simple_client_start(SimpleClient* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_start(self->threads[i]);
    }
}

void simple_client_stop(SimpleClient* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_stop(self->threads[i]);
    }
}

void simple_client_wait(SimpleClient* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_join(self->threads[i]);
    }
}

void simple_client_init_config(SimpleClientConfig* input, SimpleClientConfig* out) {
    out->io_thread_count = input->io_thread_count;
    out->io_thread_count = out->io_thread_count <= 0 ? 1 : out->io_thread_count;
}
