/**
 * TcpServer是Tcp服务器的封装
 *
 * 工作流程：
 * 1. Server创建一个Acceptor，用于监听端口或者路径，并将NewConnection回调绑定到Acceptor上面
 * 2. Server创建一组IO线程
 * 3. 当有新的链接达到时，Acceptor会回调NewConnection，NewConnection会创建一个Connection，并从IO线程组中选取一个IO线程绑定给Connection
 * 4. 当连接创建好了之后，连接会尝试回调handler的new_conn方法
 * 5. 连接会绑定读事件，一旦连接可读，连接回调read方法
 * 6. 当用户的数据接收并处理完成，需要用户在read方法中主动注册写事件绑定
 * 7. 当连接可写时，系统会调用write方法
 *
 * 一个新的fd，我们把它抽象为Connection
 * 在Connection内部，一次完整的请求和响应我们抽象为Session
 */

#ifndef SIMPLE_TCP_SERVER_H
#define SIMPLE_TCP_SERVER_H

#include "../io/io_handler.h"

// Tcp Server 相关的配置
typedef struct simple_tcp_server_config_t {
    int io_thread_count;
    // TODO 添加更多的选项，比如连接复用，超时等
} SimpleTcpServerConfig;

typedef struct simpel_tcp_server_t SimpleTcpServer;

/**
 * 创建服务器
 *
 * 参数
 *     port 需要监听的端口
 *     handler IO时间绑定
 *     config 服务器启动的相关配置，可以传入NULL
 */
// TODO 这里port可以抽象为一个Address的对象，Address对象接收端口或者domain socket的路径
SimpleTcpServer* simple_tcp_server_create(int port, SimpleIOHandler* handler, SimpleTcpServerConfig* config);

/**
 * 启动服务器
 */
void simple_tcp_server_start(SimpleTcpServer* self);

/**
 * 关闭服务器
 */
void simple_tcp_server_shutdown(SimpleTcpServer* self);

#endif