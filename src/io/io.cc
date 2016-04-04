#include "io/io.h"

#include "io/io_thread.h"
#include "io/io_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct simple_io_port_t {
    int port;
    SimpleIOThread* listen; // 专门用于监听端口的IO线程
    ArrayList* threads; // 存储IO线程的列表
    SimpleIOHandler* handler;
} SimpleIOPort;

struct simple_io_t {
    ArrayList* ports; // 监听的端口
};

SimpleIO* simple_io_create(int thread_num) {
    // TODO
    return NULL;
}

void simple_io_destory(SimpleIO* self) {
    // TODO
}

int simple_io_add_listen(SimpleIO* self, int port, SimpleIOHandler* handler) {
    // check the port exist 
    ArrayListIterator* it = arraylist_iterator(self->ports);
    while (arraylist_iterator_hasnext(it)) {
        SimpleIOPort* p = (SimpleIOPort*) arraylist_iterator_next(it);
        if (p->port == port) { // 存在则返回-1，表示添加失败
            return -1;
        }
    }
    arraylist_iterator_free(it);
    
    // add the port to the list of ports
    SimpleIOPort* p = malloc(sizeof(SimpleIOPort));
    p->port = port;
    p->listen = simple_io_thread_create();
    // TODO 需要确定一个合理的值
    p->threads = arraylist_new(1);
    // TODO 为了便于开发，暂时先注入一个IO线程
    arraylist_add(p->threads, simple_io_thread_create());
    p->handler = handler;

    // 将端口注入监听列表中
    arraylist_add(self->ports, p);
    return 0;
}

int simple_io_start(SimpleIO* self) {
    ArrayListIterator* it = arraylist_iterator(self->ports);
    while (arraylist_iterator_hasnext(it)) {
        SimpleIOPort* p = (SimpleIOPort*) arraylist_iterator_next(it);
        // 优先初始化工作的IO线程
        ArrayListIterator* it_worker = arraylist_iterator(p->threads);
        while (arraylist_iterator_hasnext(it_worker)) {
            SimpleIOThread* t = (SimpleIOThread*) arraylist_iterator_next(it_worker);
            simple_io_thread_start(t);
        }
        arraylist_iterator_free(it_worker);
        // 初始化监听的IO线程
        
        // TODO 这里应该有一个TcpAcceptor，用于监听新的连接
        simple_io_thread_add_file_event(p->listen, fd, AE_READABLE, p->handler->connect, "");
    }
    arraylist_iterator_free(it);
}
