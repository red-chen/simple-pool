#include <simple/net/define.h>
#include <simple/net/server.h>
#include <simple/net/connection.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

static bool RUNNING = true;

static void handle_sig(int sig) {
    printf("Get signal: %d\n", sig);
    RUNNING = false;
}

static int handle_new_conn(SimpleConnection* c);

static void* handle_decode(SimpleMessage* m);

static int handle_encode(SimpleConnection* c, void* data);

static int handle_process(SimpleConnection* c);

int main() {
    // 设置信号处理
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);

    // 准备Handler
    SimpleHandler handler;
    handler.new_conn = handle_new_conn;
    handler.decode = handle_decode;
    handler.encode = handle_encode;
    handler.process = handle_process;

    // 初始化配置
    SimpleServerConfig conf;
    conf.io_thread_count = 1;

    SimpleServer* s = simple_server_create(11233, &handler, &conf);

    simple_server_start(s);

    while(RUNNING) {
        sleep(1);
    }

    simple_server_stop(s);
    simple_server_wait(s);
    simple_server_destroy(s);

    return 0;
}

// ----------------------------------------------------------------
int handle_new_conn(SimpleConnection* c) {
    printf("new connection !");
    return AE_OK;
}

void* handle_decode(SimpleMessage* m) {
    return NULL;
}

int handle_encode(SimpleConnection* c, void* data) {
    return AE_OK;
}

int handle_process(SimpleConnection* c) {
    return AE_OK;
}



