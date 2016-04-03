#include <simple/net/server.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

static bool RUNNING = true;

static char buffer[1024] = {0};

static void handle_sig(int sig) {
    printf("Get signal: %d\n", sig);
    RUNNING = false;
}

static int handle_connect(EventLoop* loop, int listen_fd, void* user_data, int mask);
static int handle_read(EventLoop* loop, int fd, void* user_data, int mask);
static int handle_write(EventLoop* loop, int fd, void* user_data, int mask);

int main() {
    // 设置信号处理
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);

    // 准备Handler
    SimpleIOHandler handler;
    handler.new_conn = handle_connect;
    handler.read = handle_read;
    handler.write = handle_write;

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

// Implement file func
int handle_write(EventLoop* loop, int fd, void* user_data, int mask) {
    int n = write(fd, buffer, strlen(buffer) + 1);
    if (n > 0) {
        printf("write:%s\n", buffer);
        event_loop_add_file_event(loop, fd, AE_READABLE, handle_read, buffer);    
    } else {
        close(fd);
    }
    return AE_NOMORE;
}

// Implement file func
int handle_read(EventLoop* loop, int fd, void* user_data, int mask) {
    bzero(buffer, 1024);
    int n = read(fd, buffer, 1024);
    if (n > 0) {
        printf("read:%s\n", buffer);
        event_loop_add_file_event(loop, fd, AE_WRITABLE, handle_write, buffer);    
    } else {
        close(fd);
    }
    // 思考：如果Read的数据不完整怎么办？
    // loop提供了AE_AGAIN选项，所以可以将这个选项返回。对于真实的应用场景，这里应该在封装一层，因为
    // 数据还没有收集完整，buffer也不能交给下游，所以应该封装一个‘Session’和fd绑定，生存周期也和fd一致。
    // Session内部创建Request和Response，当单次的读数据完整之后，拼装一个Request，交给下游。
    return AE_NOMORE;
}

// Implement file func
int handle_connect(EventLoop* loop, int listen_fd, void* user_data, int mask) {
    struct sockaddr_in client_addr;
    socklen_t sockaddr_len = sizeof(struct sockaddr_in);
    int conn_fd = accept(listen_fd, (struct sockaddr*) &client_addr, (socklen_t*) &sockaddr_len);
    printf("new connection from [%s:%d] accept socket fd:%d.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), conn_fd);
    // 思考：如果我们要实现多线程这里应该怎么处理？
    // 如例子所示，一个IO线程专门用户新连接的接收，可以吧线程池放入user_data中，这里可以顺序选择线程池中的
    // 线程。比如，线程池中维护一个IO线程的列表，每个连接被建立，每次都选取一个连接，并将连接绑定到IO线程上。
    event_loop_add_file_event(loop, conn_fd, AE_READABLE, handle_read, user_data);    
    return AE_AGAIN;
}


