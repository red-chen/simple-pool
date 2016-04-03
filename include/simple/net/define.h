#ifndef SIMPLE_NET_DEFINE_H
#define SIMPLE_NET_DEFINE_H

// ----------------------------------------------
// 基础对象声明
// ----------------------------------------------

typedef struct simple_server_t SimpleServer;
typedef struct simple_acceptor_t SimpleAcceptor;
typedef struct simple_connection_t SimpleConnection;
typedef struct simple_session_t SimpleSession;
typedef struct simple_request_t SimpleRequest;
typedef struct simple_response_t SimpleResponse;
typedef struct simple_message_t SimpleMessage;

// ----------------------------------------------
// 用户事件回调方法
// ----------------------------------------------

typedef int SimpleNewConn(SimpleConnection* c);
typedef int SimpleDisConn(SimpleConnection* c);
typedef int SimpleIdleConn(SimpleConnection* c);

/**
 * 数据编码函数
 *
 * 将用户数据编码到发送的Buffer中
 */
typedef int SimpleEncode(SimpleRequest* r, void* data);

/**
 * 数据解析函数
 *
 * 该函数的主要目的是读取SimpleMessage中的数据，尝试构造为一个
 * 完整的数据包，并将数据包返回给系统。
 */
typedef void* SimpleDecode(SimpleMessage* m);
typedef int SimpleProcess(SimpleRequest* r);

typedef struct simple_handler_t {
    SimpleNewConn* new_conn; // 可选
    SimpleDisConn* dis_conn; // 可选
    SimpleIdleConn* idle_conn; // 可选
    SimpleEncode* encode;
    SimpleDecode* decode;
    SimpleProcess* process;

    // 方便用户自定义一些数据
    // 比如，确切知道一个数据包的大小，那么可以将这个size传入进去，decode函数
    // 可以根据这个size，判断是否已经足够生成一个完整的数据包
    void* user_data;
} SimpleHandler;

typedef struct simple_address_t {
    // TODO
} SimpleAddress;

typedef void SimpleNewConnection(void* user_data, int sock);

#endif
