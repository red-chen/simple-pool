#ifndef SIMPLE_NET_ACCEPTOR_H
#define SIMPLE_NET_ACCEPTOR_H

#include "define.h"

#include "io/io_thread.h"

SimpleAcceptor* simple_acceptor_create(
        int port, 
        SimpleNewConnection* cb, 
        void* user_data,
        SimpleIOThread* thread);

void simple_acceptor_start(SimpleAcceptor* self);

void simple_acceptor_stop(SimpleAcceptor* self);

void simple_acceptor_wait(SimpleAcceptor* self);

void simple_acceptor_destroy(SimpleAcceptor* self);

#endif
