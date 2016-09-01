#include "fcgi.h"
#include "fcgi_handler.h"

#include <simple/define.h>
#include <simple/address.h>
#include <simple/message.h>
#include <simple/server.h>
#include <simple/connection.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

static bool RUNNING = true;

const int FCGI_HEADER_SIZE = sizeof(FCGI_Header);

static void handle_sig(int sig) {
    printf("Get signal: %d\n", sig);
    RUNNING = false;
}

static int handle_new_conn(SimpleConnection* c);

static void* handle_decode(SimpleMessage* m);

static int handle_encode(SimpleConnection* c, void* data);

static int handle_process(SimpleConnection* c);

int main() {
    // 设置信号处理
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);

    // 准备Handler
    SimpleHandler handler;
    bzero(&handler, sizeof(SimpleHandler));

    handler.new_conn = handle_new_conn;
    handler.decode = handle_decode;
    handler.encode = handle_encode;
    handler.process = handle_process;

    // 初始化配置
    SimpleServerConfig conf;
    conf.io_thread_count = 4;

    SimpleAddress* a = simple_address_create_inet(NULL, 11233);

    SimpleServer* s = simple_server_create(a, &handler, &conf, NULL);

    simple_server_start(s);

    while(RUNNING) {
        sleep(1);
    }

    simple_server_stop(s);
    simple_server_wait(s);
    simple_server_destroy(s);
    simple_address_destroy(a);
    return 0;
}

// ----------------------------------------------------------------
int handle_new_conn(SimpleConnection* c) {
    SimpleAddress* r = simple_connection_get_client(c);
    SimpleAddress* l = simple_connection_get_server(c);

    printf("new connection [%s:%d --> %s:%d], fd: %d \n", 
            simple_address_get_addr(r),
            simple_address_get_port(r),
            simple_address_get_addr(l),
            simple_address_get_port(l),
            simple_connection_get_fd(c));
    return AE_OK;
}

/**
 * parse FCGI_BeginRequestBody
 */
static int fcgi_parse_begin_request(void* data, int size) {
    // TODO assert size == sizeof(FCGI_BeginRequestBody)

    FCGI_BeginRequestBody body;
    memcpy(&body, data, size);
    if (fcgi_get_role(&body) == FCGI_RESPONDER) {
        // TODO set connection keep alive ?
        return 0;
    }
    return -1;
}

//static int fcgi_parse_params(void* data, int size) {
//    if(size > 0) {
//        
//    } else if (size == 0) {
//        
//    } 
//    return 0;
//}

void* handle_decode(SimpleMessage* m) {
    int size = simple_message_size(m);
    if (size >= FCGI_HEADER_SIZE) {
        FCGI_Header* header = (FCGI_Header*)simple_message_get_pull_ptr(m);

        int total = FCGI_HEADER_SIZE + fcgi_get_content_length(header) + header->paddingLength;
        if (size >= total) {
            return header;
        }
        int ret = 0;
        if (size >= total) {
            switch (header->type) {
                case FCGI_BEGIN_REQUEST:
                    ret = fcgi_parse_begin_request(
                            simple_message_get_pull_ptr(m) + FCGI_HEADER_SIZE, 
                            fcgi_get_content_length(header) + header->paddingLength
                    );
                    break; 
                case FCGI_PARAMS: 

                    break;
                case FCGI_STDIN: break;
                case FCGI_STDOUT: break;
                case FCGI_STDERR: break;

                case FCGI_ABORT_REQUEST: 
                case FCGI_END_REQUEST: 
                case FCGI_DATA: 
                case FCGI_GET_VALUES: 
                case FCGI_GET_VALUES_RESULT: 
                case FCGI_UNKNOWN_TYPE: 
                default: break;
            }
            // 如果操作成功，那么移动buffer
            if (ret == 0) {
                simple_message_set_pull_size(m, total);
            }
        }
    }
    return NULL;
}

int handle_process(SimpleConnection* c) {
    SimpleIOThread* t = simple_connection_get_thread(c);
    SimpleMessage* in = simple_connection_get_in(c);

    printf("fd: %d, data: %s, use io thread: %s \n", 
            simple_connection_get_fd(c),
            (char*)simple_message_get(in), 
            (char*)simple_io_thread_get_name(t));


    // handler(request, response);

    simple_connection_send(c, simple_message_get(in));
    return AE_OK;
}

int handle_encode(SimpleConnection* c, void* data) {
    SimpleMessage* out = simple_connection_get_out(c);
    simple_message_add(out, data, strlen(data) + 1);
    return AE_OK;
}


