#include "net/connection.h"

struct simple_connection_t {
    int sock;
    SimpleIOThread* thread;
    SimpleIOHandler* handler;
};

SimpleConnection* simple_connection_create(SimpleIOThread* thread, int sock, SimpleIOHandler* handler) {
    SimpleConnection* self = malloc(sizeof(SimpleConnection));
    self->sock = sock;
    self->thread = thread;
    self->handler = handler;
    return self;
}

void simple_connection_establish(SimpleConnection* self) {
    if (self->handler->connect != NULL) {
        self->handler->connect(self);
    }

    // TODO 如果仅仅发送数据，是否需要注册读事件回调
    simple_io_thread_add_file_event(
        self->thread,
        self->sock,
        AE_READABLE,
        self->handler->read,
        self
    );
}

void simple_connection_close(SimpleConnection* self) {
    // TODO
}
