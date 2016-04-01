#ifndef SIMPLE_CONNECTION_H
#define SIMPLE_CONNECTION_H

#include "../io/io_handler.h"

typedef struct simple_connection_t SimpleConnection;

/**
 *  创建连接，绑定sock和IO线程
 */
SimpleConnection* simple_connection_create(IOThread* thread, int sock, SimpleIOHandler* handler);

void simple_connection_establish(SimpleConnection* self);

void simple_connection_close(SimpleConnection* self)

#endif
