/**
 * 地址定义
 */
#ifndef SIMPLE_ADDRESS_H
#define SIMPLE_ADDRESS_H

typedef struct simple_address_t {
    char path[108]; // define from unix struct sockaddr_un
} SimpleAddress;

#endif
