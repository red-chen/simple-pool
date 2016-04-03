#include "net/connection.h"

struct simple_connection_t {
    int sock;
    SimpleIOThread* thread;
    SimpleHandler* handler;

    //
    SimpleSession* session;
};

struct simple_session_t {
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

int simple_connection_send(SimpleConnection* self, SimpleRequest* r) {
    // encode
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
   
    return AE_NOMORE;
}

int simple_connection_write(EventLoop* loop, int fd, void* user_data, int mask) {
    SimpleConnection* self = (SimpleConnection*) user_data;

    // 读取发送队列中的数据，将数据发送出去，同理，也只调用一次write

    return AE_NOMORE;
}


