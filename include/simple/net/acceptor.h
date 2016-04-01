#ifndef SIMPLE_ACCEPTOR_H
#define SIMPLE_ACCEPTOR_H

typedef struct simple_acceptor_t SimpleAcceptor;

SimpleAcceptor* simple_acceptor_create(int port, NewConnectionCB* cb);

void simple_acceptor_destroy(SimpleAcceptor* self);

void simple_acceptor_start(SimpleAcceptor* self);

void simple_acceptor_stop(SimpleAcceptor* self);

#endif
