# Simple-Poll
基于Event Loop实现的基础网络框架，可以很容易基于此框架实现各种高性能的网络服务器

[![Software License](https://img.shields.io/badge/license-apache2-brightgreen.svg)](LICENSE)
[![Build Status](https://api.travis-ci.org/red-chen/simple-pool.svg)](https://travis-ci.org/red-chen/simple-pool)

# 开发环境
* gcc version 4.8.3 20140911 (Red Hat 4.8.3-9) (GCC)
* C99
* python
* Linux

# 本地编译
```
# 安装scons
https://github.com/red-chen/scons-init

# 编译代码
> git clone https://github.com/red-chen/simple-pool.git --recursive
> cd simple-pool
# option
> git submodule update --recursive
> cd deps/simple
> scons
> cd -
> scons
```
# 启动流程
## Server端
## Client端
1. 配置SimpleHandler
2. 创建Client端对象
3. 创建连接
4. 启动Client端
5. 等待Client退出

```
    // 准备Handler
    SimpleHandler handler;
    bzero(&handler, sizeof(SimpleHandler));

    handler.new_conn = handle_new_conn;
    handler.new_packet = handle_new_packet;
    handler.decode = handle_decode;
    handler.encode = handle_encode;
    handler.process = handle_process;

    // 初始化配置
    SimpleClientConfig conf;
    conf.io_thread_count = io_count;

    SimpleAddress* a = simple_address_create_inet("127.0.0.1", 11233);
    SimpleClient* s = simple_client_create(a, &handler, &conf);

    for (int i = 0; i < limit; i++) {
        simple_client_connect(s);
    }

    simple_client_start(s);

    while(RUNNING && result < limit) {
        sleep(1);
    }

    simple_client_stop(s);
    simple_client_wait(s);
    simple_client_destroy(s);
    simple_address_destroy(a);
```
