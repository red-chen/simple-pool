#ifndef SIMPLE_NET_CLIENT_H
#define SIMPLE_NET_CLIENT_H

#include "define.h"

typedef struct simple_client_config_t {
    int io_thread_count;
} SimpleClientConfig;

SimpleClient* simple_client_create(
        const char* host,
        int port, 
        SimpleHandler* handler, 
        SimpleClientConfig* config);

void simple_client_start(SimpleClient* self);

void simple_client_stop(SimpleClient* self);

void simple_client_wait(SimpleClient* self);

void simple_client_destroy(SimpleClient* self);

#endif
