#include "net/connection.h"
#include "net/message.h"

#include "io_thread.h"
#include "event_loop.h"
#include "assert.h"

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

    // 存储接收到的数据
    SimpleMessage* in; 
    // 当数据满足大小需要时，就将message中的数据构建了一个对象
    void* in_data; 
    // 存储将要发送的数据
    SimpleMessage* out;
    // 存储用户写入数据的指针，系统之后会调用encode函数，将数据写入out message中
    void* out_data;
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
    ASSERT(sock > 0, "input fd invalid, fd:%d", sock);

    SimpleConnection* self = malloc(sizeof(SimpleConnection));
    self->sock = sock;
    self->thread = thread;
    self->handler = handler;
    self->in = simple_message_create();
    self->out = simple_message_create();
    return self;
}

void simple_connection_close(SimpleConnection* self) {
    if (self->handler->dis_conn != NULL) {
        self->handler->dis_conn(self);
    }
    
    close(self->sock);

    simple_message_destroy(self->in);
    simple_message_destroy(self->out);

    free(self);
}

void simple_connection_establish(SimpleConnection* self) {
    if (self->handler->new_conn != NULL) {
        self->handler->new_conn(self);
    }

    simple_io_thread_add_file_event(
        self->thread,
        self->sock,
        AE_READABLE,
        simple_connection_read,
        self
    );
    
    simple_io_thread_add_file_event(
        self->thread,
        self->sock,
        AE_WRITABLE,
        simple_connection_write,
        self
    );
}

int simple_connection_send(SimpleConnection* self, void* data) {
    // TODO 可以通过connection申请data内存，降低拷贝次数

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

// ---------------------------------------------------

int simple_connection_read(EventLoop* loop, int fd, void* user_data, int mask) {
    SimpleConnection* self = (SimpleConnection*) user_data;
    // NOTE 怎么知道要读多少数据呢？
    // 比如一个结构，因为用户才知道要读多少数据，所以这里应该嵌入用户的逻辑
    // 代码，所以这里应该调用handler的decode方法。
    
    // NOTE 这里使用一个Buffer来接收数据，Buffer每次调用一下read，然后将收到的数据
    // 传送给handler中decode函数, decode在函数在根据收到数据的大小决定放回
    // 的内容
    
    // NOTE 为什么每次只调用一次read呢？
    // 因为我们的Buffer接收大小默认开启为16KB，单次足够满足大多数的请求需要，为了
    // 不让某个大请求持续的占用线程，所以不能持续读取数据

    // TODO Buffer大小可以动态设置, 考虑Buffer动态在堆上分配或者编译决定大小
  
    void* buffer = simple_message_get_push_ptr(self->in, SIMPLE_READ_BUF_SIZE);
    int n = read(fd, buffer, SIMPLE_READ_BUF_SIZE);
    if (n > 0) {
        simple_message_set_push_size(self->in, n);
        // build request
        void* data = self->handler->decode(self->in);
        if (data == NULL) {
            return AE_AGAIN;
        }
        self->handler->process(self);
    } else {
        simple_connection_close(self);
    }
    return AE_NOMORE;
}

int simple_connection_write(EventLoop* loop, int fd, void* user_data, int mask) {
    SimpleConnection* self = (SimpleConnection*) user_data;

    // new_packet
    if (self->handler->new_packet != NULL) {
        self->handler->new_packet(self);
    }

    void* data = simple_message_get_pull_ptr(self->out);
    int size = simple_message_size(self->out);

    // TODO 考虑是否需要检查size ?
    if (size <= 0 ) {
        return AE_NOMORE;
    }

    // NOTE 读取发送队列中的数据，将数据发送出去，同理，也只调用一次write
    int n = write(fd, data, size);

    if (n > 0) {
        if (n >= size) {
            simple_io_thread_add_file_event(
                self->thread,
                self->sock,
                AE_READABLE,
                simple_connection_read,
                self
            );
        } else {
            simple_message_set_pull_size(self->out, n);
            return AE_AGAIN;
        }
    } else {
        simple_connection_close(self);
    }
    return AE_NOMORE;
}

SimpleIOThread* simple_connection_get_thread(SimpleConnection* self) {
    return self->thread;
}

SimpleMessage* simple_connection_get_in(SimpleConnection* self) {
    return self->in;
}

SimpleMessage* simple_connection_get_out(SimpleConnection* self) {
    return self->out;
}

int simple_connection_get_fd(SimpleConnection* self) {
    return self->sock;
}
