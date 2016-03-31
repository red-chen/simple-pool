#ifndef SIMPLE_TCP_ACCEPTOR_H
#define SIMPLE_TCP_ACCEPTOR_H

typedef struct simple_tcp_acceptor_t SimpleTcpAcceptor;

SimpleTcpAcceptor* simple_tcp_acceptor_create(int port, NewConnectionCB* cb);

void simple_tcp_acceptor_start(SimpleTcpAcceptor* self);

void simple_tcp_acceptor_stop(SimpleTcpAcceptor* self);

#endif
