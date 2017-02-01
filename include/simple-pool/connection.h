#ifndef SIMPLE_NET_CONNECTION_H
#define SIMPLE_NET_CONNECTION_H

#include "define.h"

#include "lemon/io_thread.h"
#include "lemon/timestamp.h"

SimpleConnection* simple_connection_create(
        SimpleIOThread* thread, 
        int sock, 
        SimpleHandler* handler,
        SimpleAddress* client,
        SimpleAddress* server);

void simple_connection_establish(SimpleConnection* self);

int simple_connection_send(SimpleConnection* self, void* data);

void simple_connection_close(SimpleConnection* self);

SimpleIOThread* simple_connection_get_thread(SimpleConnection* self);

SimpleMessage* simple_connection_get_in(SimpleConnection* self);

SimpleMessage* simple_connection_get_out(SimpleConnection* self);

int simple_connection_get_fd(SimpleConnection* self);

SimpleAddress* simple_connection_get_client(SimpleConnection* self);

SimpleAddress* simple_connection_get_server(SimpleConnection* self);

TIME_IN_MICRO simple_connection_get_create_time(SimpleConnection* self);

int64_t simple_connection_get_in_flow_bytes(SimpleConnection* self);

int64_t simple_connection_get_out_flow_bytes(SimpleConnection* self);

#endif
