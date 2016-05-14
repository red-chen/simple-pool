#ifndef FCGI_HANDLER_H
#define FCGI_HANDLER_H

#include "fcgi.h"

void fcgi_header_init(int type, int requestId, int contentLength, int paddingLength, /*out*/FCGI_Header* header);

void fcgi_begin_request_body_init(int role, int keepConn, /*out*/FCGI_BeginRequestBody* body);

void fcgi_end_request_body_init(int appStatus, int protocalStatus, /*out*/FCGI_EndRequestBody* body);

void fcgi_begin_request_record_init(
        int type, int requestId, int contentLength, int paddingLength,
        int role, int keepConn,
        /*out*/FCGI_BeginRequestRecord* record
    );

void fcgi_end_request_record_init(
        int type, int requestId, int contentLength, int paddingLength,
        int appStatus, int protocolStatus,
        /*out*/FCGI_EndRequestRecord* record
    );

int fcgi_get_request_id(FCGI_Header* header);

int fcgi_get_content_length(FCGI_Header* header);

int fcgi_get_role(FCGI_BeginRequestBody* body);

int fcgi_get_app_status(FCGI_EndRequestBody* body);

void fcgi_add_param(const char* name, const char* value, /*out*/void* buf);

int fcgi_get_length();

#endif
