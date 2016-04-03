#ifndef SIMPLE_NET_ACCEPTOR_H
#define SIMPLE_NET_ACCEPTOR_H

#include "define.h"

SimpleAcceptor* simple_acceptor_create(int port, SimpleNewConnection* cb, SimpleIOThread* thread);

void simple_acceptor_start(SimpleAcceptor* self);

void simple_acceptor_stop(SimpleAcceptor* self);

void simple_acceptor_wait(SimpleAcceptor* self);

void simple_acceptor_destroy(SimpleAcceptor* self);

#endif
