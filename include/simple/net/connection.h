#ifndef SIMPLE_NET_CONNECTION_H
#define SIMPLE_NET_CONNECTION_H

#include "define.h"

/**
 *  创建连接，绑定sock和IO线程
 */
SimpleConnection* simple_connection_create(
        IOThread* thread, 
        int sock, 
        SimpleHandler* handler);

/**
 * 构建连接
 *
 * 并注册可读事件
 */
void simple_connection_establish(SimpleConnection* self);

/**
 * 发送数据
 */
int simple_connection_send(SimpleConnection* self, SimpleRequest* r);

void simple_connection_close(SimpleConnection* self)

#endif
