/**
 * IO本质上就是一个由IO Thread组成的IO多线程池。
 *
 * 线程池会启动一个独立的线程，用于监听端口。当有新的连接到达时，系统会
 * 顺序选择一个IO线程，将新的fd分派给IO线程。
 *
 * TODO 需要检查当新的fd达到时，放入别的loop中时，是否有问题
 */
#ifndef IO_H
#define IO_H

#include "io_handler.h"

typedef struct simpel_io_config_t {
    // 单个连接的生存周期，取值为>0 或者 -1， -1表示一直存在直
    // 到客户端主动退出，默认为-1
    int alive_in_sec; 
    // 回收空闲连接的间隔周期，默认是8秒
    int recycle_interval_in_sec;
} SimpleIOConfig;

typedef struct simple_io_t SimpleIO;

/**
 * 创建IO线程池
 *
 * 输入参数
 *     thread_num ：IO线程的数目，取值范围为 > 0 或者 -1，-1表示CPU核数个
 */
SimpleIO* simple_io_create(int thread_num);

void simple_io_destory(SimpleIO* self);

/**
 * 添加端口和handler绑定
 * 这里可以实现多个端口的监听
 *
 * 输入参数
 *     port ： 需要监听的端口
 *     handler: 处理各种事件的函数
 *
 * 备注，系统内部会至少启动2个IO线程，其中一个线程用户Acceptor新建连接，并把
 * 新建连接放到工作的IO线程中。
 */
int simple_io_add_listen(SimpleIO* self, int port, SimpleIOHandler* handler);

int simple_io_start(SimpleIO* self);

int simple_io_join(SimpleIO* self);

#endif
