#include <simple/net/define.h>
#include <simple/net/address.h>
#include <simple/net/message.h>
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
    bzero(&handler, sizeof(SimpleHandler));

    handler.new_conn = handle_new_conn;
    handler.decode = handle_decode;
    handler.encode = handle_encode;
    handler.process = handle_process;

    // 初始化配置
    SimpleServerConfig conf;
    conf.io_thread_count = 4;

    SimpleAddress* a = simple_address_create_inet(NULL, 11233);

    SimpleServer* s = simple_server_create(a, &handler, &conf, NULL);

    simple_server_start(s);

    while(RUNNING) {
        sleep(1);
    }

    simple_server_stop(s);
    simple_server_wait(s);
    simple_server_destroy(s);
    simple_address_destroy(a);
    return 0;
}

// ----------------------------------------------------------------
int handle_new_conn(SimpleConnection* c) {
    SimpleAddress* r = simple_connection_get_client(c);
    SimpleAddress* l = simple_connection_get_server(c);

    printf("new connection [%s:%d --> %s:%d], fd: %d \n", 
            simple_address_get_addr(r),
            simple_address_get_port(r),
            simple_address_get_addr(l),
            simple_address_get_port(l),
            simple_connection_get_fd(c));
    return AE_OK;
}

void* handle_decode(SimpleMessage* m) {
    // NOTE 比如我们的接收缓冲区只有4KB，但是有4M的数据要发送过来，这时
    // 我们需要从Connection的MemoryPool中申请一段内存，用于存储大量的数据。
    // 这段内存的释放，用户不用关心，当单次请求响应完成之后，系统会主动释
    // 放内存空间。

    // NOTE 我们的数据是流式的数据，比如单个包是1KB，一共要发送1024个包，我
    // 们不想等待所有的包发送完毕之后再处理，比如，一旦一个完整的包达到，我
    // 们就可以处理一个包，这里应该怎么办？
    // 针对于这种情况，建议用户自己在decode函数中判断一个包是否完成，如果包
    // 已经构建好，那么可以将这个包发送到一个队里中，或者一旦接到一部分数据
    // 将这个数据写入一个stream钟，下游线程从队列或者stream中获取数据处理。
    return simple_message_get_pull_ptr(m);
}

int handle_process(SimpleConnection* c) {
    SimpleIOThread* t = simple_connection_get_thread(c);
    SimpleMessage* in = simple_connection_get_in(c);

    printf("fd: %d, data: %s, use io thread: %s \n", 
            simple_connection_get_fd(c),
            (char*)simple_message_get(in), 
            (char*)simple_io_thread_get_name(t));

    simple_connection_send(c, simple_message_get(in));
    return AE_OK;
}

int handle_encode(SimpleConnection* c, void* data) {
    SimpleMessage* out = simple_connection_get_out(c);
    simple_message_add(out, data, strlen(data) + 1);
    return AE_OK;
}


