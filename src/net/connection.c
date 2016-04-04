#include "net/connection.h"

#include "net/message.h"
#include "io/io_thread.h"

#include "event_loop.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define SIMPLE_READ_BUF_SIZE 16 * 1024 //

struct simple_connection_t {
    int sock;
    SimpleIOThread* thread;
    SimpleHandler* handler;

    SimpleMessage* message;
    void* input_data; // 存储接收到的数据
    void* output_data; // 存储将要发送的数据
};

static int simple_connection_read(
        EventLoop* loop, 
        int fd, 
        void* user_data, 
        int mask);

static int simple_connection_write(
        EventLoop* loop, 
        int fd, 
        void* user_data, 
        int mask);

SimpleConnection* simple_connection_create(
        SimpleIOThread* thread, 
        int sock, 
        SimpleHandler* handler) {
    SimpleConnection* self = malloc(sizeof(SimpleConnection));
    self->sock = sock;
    self->thread = thread;
    self->handler = handler;
    return self;
}

void simple_connection_establish(SimpleConnection* self) {
    if (self->handler->new_conn != NULL) {
        self->handler->new_conn(self);
    }

    // TODO 如果仅仅发送数据，是否需要注册读事件回调
    simple_io_thread_add_file_event(
        self->thread,
        self->sock,
        AE_READABLE,
        simple_connection_read,
        self
    );
}

int simple_connection_send(SimpleConnection* self, void* data) {
    // encode
    self->handler->encode(self, data);

    simple_io_thread_add_file_event(
        self->thread,
        self->sock,
        AE_WRITABLE,
        simple_connection_write,
        self
    );

    return AE_OK;
}

void simple_connection_close(SimpleConnection* self) {
    if (self->handler->dis_conn != NULL) {
        self->handler->dis_conn(self);
    }
    // TODO
}

// ---------------------------------------------------

int simple_connection_read(EventLoop* loop, int fd, void* user_data, int mask) {
    SimpleConnection* self = (SimpleConnection*) user_data;
    // TODO 怎么知道要读多少数据呢？
    // 比如一个结构，因为用户才知道要读多少数据，所以这里应该嵌入用户的逻辑
    // 代码，所以这里应该调用handler的decode方法。
    
    // TODO 这里使用一个Buffer来接收数据，Buffer每次调用一下read，然后将收到的数据
    // 传送给handler中decode函数, decode在函数在根据收到数据的大小决定放回
    // 的内容
    
    // NOTE 为什么每次只调用一次read呢？
    // 因为我们的Buffer接收大小默认开启为16KB，单次足够满足大多数的请求需要，为了
    // 不让某个大请求持续的占用线程，所以不能持续读取数据
    // TODO Buffer大小可以动态设置, 考虑Buffer动态在堆上分配或者编译决定大小
  
    void* buffer = simple_message_pointer(self->message, SIMPLE_READ_BUF_SIZE);
    int n = read(fd, buffer, SIMPLE_READ_BUF_SIZE);
    if (n > 0) {
        simple_message_move(self->message, n);
        // build request
        void* data = self->handler->decode(self->message);
        if (data == NULL) {
            return AE_AGAIN;
        }
        self->input_data = data;
        self->handler->process(self);
    } else {
        simple_connection_close(self);
    }

    return AE_NOMORE;
}

int simple_connection_write(EventLoop* loop, int fd, void* user_data, int mask) {
    SimpleConnection* self = (SimpleConnection*) user_data;

    // 读取发送队列中的数据，将数据发送出去，同理，也只调用一次write
    // TODO 为了开发简单，仅仅调用一次write，后面重构
    int n = write(fd, self->output_data, strlen(self->output_data) + 1);
    if (n > 0) {
        simple_io_thread_add_file_event(
            self->thread,
            self->sock,
            AE_READABLE,
            simple_connection_read,
            self
        );
    } else {
        simple_connection_close(self);
    }

    return AE_NOMORE;
}
