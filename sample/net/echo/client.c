/**
 * Client的使用方式
 *
 * 用户是生产者，生产数据（请求）之后，将请求和回调放入队列中。Client的IO线程当
 * 事件可写时，从队列中取得一个请求，发送到服务器，当请求响应完毕时，将请求、响
 * 应和回调一起放入队列中，外层工作线程来调用回调。
 *
 * 对于底层的网络库来说，上层的生产者应该是异步框架。异步框架在接收到用户的请求
 * 之后，产生一个Task，并将Task中的请求和回调传入队列中，当底层网络框架响应之后
 * 异步框架调用Task的“状态”函数，通知响应已经完成或者调用回调函数。
 */

#include <simple/net/define.h>
#include <simple/net/message.h>
#include <simple/net/client.h>
#include <simple/net/connection.h>

#include <simple/atomic.h>

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

static int handle_new_packet(SimpleConnection* c);

static void* handle_decode(SimpleMessage* m);

static int handle_encode(SimpleConnection* c, void* data);

static int handle_process(SimpleConnection* c);

static int count = 0;

int main() {
    // 设置信号处理
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);

    // 准备Handler
    SimpleHandler handler;
    bzero(&handler, sizeof(SimpleHandler));

    handler.new_conn = handle_new_conn;
    handler.new_packet = handle_new_packet;
    handler.decode = handle_decode;
    handler.encode = handle_encode;
    handler.process = handle_process;

    // 初始化配置
    SimpleClientConfig conf;
    conf.io_thread_count = 2;

    SimpleClient* s = simple_client_create("127.0.0.1", 11233, &handler, &conf);


    for (int i = 0; i < 10; i++) {
        simple_client_connect(s);
    }

    simple_client_start(s);

    while(RUNNING && count < 10) {
        sleep(1);
    }

    simple_client_stop(s);
    simple_client_wait(s);
    simple_client_destroy(s);

    return 0;
}

// ----------------------------------------------------------------


int handle_new_conn(SimpleConnection* c) {
    printf("new connection\n");
    return AE_OK;
}

int handle_new_packet(SimpleConnection* c) {
    if (count > 10) {
        return AE_OK;   
    }

    ATOMIC_INC(&count);

    simple_connection_send(c, "hello world");
    return AE_OK;
}

void* handle_decode(SimpleMessage* m) {
    printf("handle_decode\n");
    printf("data:%s\n", (char*)simple_message_get_pull_ptr(m));
    return simple_message_get_pull_ptr(m);
}

int handle_encode(SimpleConnection* c, void* data) {
    printf("handle_encode\n");
    SimpleMessage* out = simple_connection_get_out(c);
    simple_message_add(out, data, strlen(data) + 1);
    printf("handle_encode end\n");
    return AE_OK;
}

int handle_process(SimpleConnection* c) {
    printf("handle_process\n");
    SimpleIOThread* t = simple_connection_get_thread(c);
    printf("use io thread: %s \n", simple_io_thread_get_name(t));
    SimpleMessage* in = simple_connection_get_in(c);
    printf("data:%s\n", (char*)simple_message_get_pull_ptr(in));
    return AE_OK;
}
