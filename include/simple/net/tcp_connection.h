#ifndef SIMPLE_TCP_CONNECTION_H
#define SIMPLE_TCP_CONNECTION_H

#include "../io/io_handler.h"

typedef struct simple_tcp_connection_t SimpleTcpConnection;

SimpleTcpConnection* simple_tcp_connection_create(IOThread* thread, int sock, SimpleIOHandler* handler);

void simple_tcp_connection_established(SimpleTcpConnection* self);

void simple_tcp_connection_close(SimpleTcpConnection* self)

#endif
