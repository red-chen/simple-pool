#include "fcgi.h"
#include "fcgi_handler.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main() {
    // init
    int requestId = 1;

    FCGI_BeginRequestRecord beginRecord;
    fcgi_header_init(FCGI_BEGIN_REQUEST, requestId, sizeof(beginRecord.body), 0,  &beginRecord.header);
    fcgi_begin_request_body_init(FCGI_RESPONDER, false, &beginRecord.body);
    
    printf("requestId:%d, contentLength:%d, role:%d \n", 
            fcgi_get_request_id(&beginRecord.header),
            fcgi_get_content_length(&beginRecord.header),
            fcgi_get_role(&beginRecord.body)
            ); 
   

    // TODO to buffer
    
    switch (beginRecord.header.type) {
        case FCGI_BEGIN_REQUEST: break; // 获取角色和连接状态
        case FCGI_ABORT_REQUEST: break;
        case FCGI_END_REQUEST: break;
        case FCGI_PARAMS: break;
        case FCGI_STDIN: break;
        case FCGI_STDOUT: break;
        case FCGI_STDERR: break;
        case FCGI_DATA: break;
        case FCGI_GET_VALUES: break;
        case FCGI_GET_VALUES_RESULT: break;
        case FCGI_UNKNOWN_TYPE: break;
        default: break;
    }
    return 0;
}
