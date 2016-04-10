#include "net/client.h"

#include "net/connection.h"

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

struct simple_client_t {
    char host[64];
    int port;
    int index;
    SimpleHandler* handler;
    SimpleClientConfig config;
    SimpleIOThread** threads;
};

static void simple_client_init_config(SimpleClientConfig* input, SimpleClientConfig* out);

SimpleClient* simple_client_create(
        const char* host, 
        int port, 
        SimpleHandler* handler, 
        SimpleClientConfig* config) {
    SimpleClient* self = malloc(sizeof(SimpleClient));
    strcpy(self->host, host);
    self->port = port;
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
    // TODO
}

int simple_client_connect(SimpleClient* self) {
    // 
    struct sockaddr_in server_addr, client_addr;
    bzero(&server_addr, sizeof(server_addr));
    bzero(&client_addr, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(self->host);
    server_addr.sin_port = htons(self->port);

    int conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // TODO check succ
    connect(conn_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    SimpleConnection* conn = simple_connection_create(
            self->threads[self->index], 
            conn_fd, 
            self->handler);

    simple_connection_establish(conn);

    return 0;
}

void simple_client_start(SimpleClient* self) {
    for (int i = 0; i < self->config.io_thread_count; i++) {
        simple_io_thread_start(self->threads[i]);
    }
}

void simple_client_stop(SimpleClient* self) {
    // TODO
}

void simple_client_wait(SimpleClient* self) {
    // TODO
}

void simple_client_init_config(SimpleClientConfig* input, SimpleClientConfig* out) {
    out->io_thread_count = input->io_thread_count;
    out->io_thread_count = out->io_thread_count <= 0 ? 1 : out->io_thread_count;
}
