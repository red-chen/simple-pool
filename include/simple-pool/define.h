#ifndef SIMPLE_NET_DEFINE_H
#define SIMPLE_NET_DEFINE_H

#include <arpa/inet.h>

// ----------------------------------------------
// 基础对象声明
// ----------------------------------------------

typedef struct SimpleClient SimpleClient;
typedef struct SimpleAddress SimpleAddress;
typedef struct SimpleServer SimpleServer;
typedef struct SimpleAcceptor SimpleAcceptor;
typedef struct SimpleConnection SimpleConnection;
typedef struct SimpleMessage SimpleMessage;

// ----------------------------------------------
// 用户事件回调方法
// ----------------------------------------------

typedef int SimpleNewConn(SimpleConnection* c);
typedef int SimpleDisConn(SimpleConnection* c);
typedef int SimpleIdleConn(SimpleConnection* c);

/**
 * @brief 新的数据包发送函数
 * 对于Client端来说，当连接可写时，将数据发送给服务器。
 * 对于Server端来说，当连接可写时，将数据发送给客户端。
 *
 * 比如，在客户端，用户将数据写入到一个队列中，当连接可写时，会调用
 * NewPacket函数，那么在NewPacket函数内部，就可以从队列中获取数据，
 * 并调用send函数，将数据发送到服务器。
 * 同理，对于服务器来说，当系统将完整的Request交给用户（线程）之后，
 * 用户处理完成之后，可以将数据写入到队列中，NewPacket函数同样获取
 * 数据，然后将数据发送给客户端。
 *
 * @param c
 *
 * @return 
 */
typedef int SimpleNewPacket(SimpleConnection* c);

/**
 * 数据编码函数
 *
 * 将用户数据打包到发送的Buffer中
 */
typedef int SimpleEncode(SimpleConnection* r, void* data);

/**
 * 数据解析函数
 *
 * 该函数的主要目的是读取SimpleMessage中的数据，尝试构造为一个
 * 完整的数据包，并将数据包返回给系统。
 *
 * 比如一个完整的Http请求等
 *
 * 调用位置：connection的read事件函数中
 *
 * 函数将接收的数据返回给上层调用，如果接收的数据未完整，函数应
 * 该返回NULL，当上层函数检查有数据返回，会调用SimpleProcess，如
 * 果返回NULL，程序会进入AE_AGAIN状态。
 */

typedef void* SimpleDecode(SimpleMessage* m);

/**
 * @brief 程序处理函数
 * 对于Client端来说，当Client端收到完整的Response之后，会调用该函数
 * 对于Server端来说，当Server端收到完整的Request之后，会调用该函数
 *
 * @param s 连接对象
 *
 * @return 
 *     0 表示处理正常，非零表示失败
 */
typedef int SimpleProcess(SimpleConnection* s);

typedef struct SimpleHandler {
    SimpleNewConn* new_conn; // 可选
    SimpleDisConn* dis_conn; // 可选
    SimpleIdleConn* idle_conn; // 可选
    SimpleNewPacket* new_packet; // 可选
    SimpleEncode* encode;
    SimpleDecode* decode;
    SimpleProcess* process;

    // 方便用户自定义一些数据
    // 比如，确切知道一个数据包的大小，那么可以将这个size传入进去，decode函数
    // 可以根据这个size，判断是否已经足够生成一个完整的数据包
    void* user_data;
} SimpleHandler;

typedef void SimpleNewConnection(
        void* user_data, 
        int sock, 
        SimpleAddress* client,
        SimpleAddress* server);

#endif
