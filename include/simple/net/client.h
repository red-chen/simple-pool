#ifndef SIMPLE_NET_CLIENT_H
#define SIMPLE_NET_CLIENT_H

#include "define.h"

typedef struct simple_client_config_t {
    int io_thread_count;
    /// 是否开启连接复用，1表示打开，0表示关闭
    //int conn_reuse;
    /// 可以创建的最大连接数
    //int max_conn_count;
    //int socket_timeout_in_sec;
    //int conn_timeout_in_sec;
    //int recycle_interval_in_sec;
} SimpleClientConfig;

SimpleClient* simple_client_create(
        const char* host,
        int port, 
        SimpleHandler* handler, 
        SimpleClientConfig* config);


/**
 * @brief 创建连接
 *
 * @param self
 *
 * @return
 *     0表示创建成功，非零表示创建失败
 */
int simple_client_connect(SimpleClient* self);

void simple_client_start(SimpleClient* self);

void simple_client_stop(SimpleClient* self);

void simple_client_wait(SimpleClient* self);

void simple_client_destroy(SimpleClient* self);

#endif
