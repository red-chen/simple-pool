#ifndef SIMPLE_NET_CONNECTION_H
#define SIMPLE_NET_CONNECTION_H

#include "define.h"

/**
 *  创建连接，绑定sock和IO线程
 */
SimpleConnection* simple_connection_create(IOThread* thread, int sock, SimpleHandler* handler);

void simple_connection_establish(SimpleConnection* self);

void simple_connection_close(SimpleConnection* self)

#endif
