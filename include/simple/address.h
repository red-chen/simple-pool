#ifndef SIMPLE_ADDRESS_H
#define SIMPLE_ADDRESS_H

#include "define.h"

/**
 * @brief 创建IPV4的Address对象
 *
 * @return 
 */
SimpleAddress* simple_address_create_inet(const char* ip, int port);

/**
 * @brief 创建IPV6的Address对象
 *
 * @return 
 */
SimpleAddress* simple_address_create_inet6(const char* ip, int port);

/**
 * @brief 创建Domain Socket的Address对象
 *
 * @return 
 */
SimpleAddress* simple_address_create_unix(const char* unix_path);

int simple_address_get_family(SimpleAddress* self);

const char* simple_address_get_addr(SimpleAddress* self);

int simple_address_get_port(SimpleAddress* self);

const char* simple_address_get_unix_path(SimpleAddress* sef);

void simple_address_destroy(SimpleAddress* self);

#endif
