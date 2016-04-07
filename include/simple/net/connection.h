#ifndef SIMPLE_NET_CONNECTION_H
#define SIMPLE_NET_CONNECTION_H

#include "define.h"

#include "io/io_thread.h"
/**
 *  创建连接，绑定sock和IO线程
 */
SimpleConnection* simple_connection_create(
        SimpleIOThread* thread, 
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
int simple_connection_send(SimpleConnection* self, void* data);

void simple_connection_close(SimpleConnection* self);

SimpleIOThread* simple_connection_get_thread(SimpleConnection* self);

#endif
