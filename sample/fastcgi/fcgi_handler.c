#include "fcgi_handler.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void fcgi_header_init(int type, int requestId, int contentLength, int paddingLength, /*out*/FCGI_Header* header) {
    header->version          = FCGI_VERSION_1;
    header->type             = (unsigned char) type;
    header->requestIdB1      = (unsigned char) ((requestId     >> 8) & 0xff);
    header->requestIdB0      = (unsigned char) ((requestId         ) & 0xff);
    header->contentLengthB1  = (unsigned char) ((contentLength >> 8) & 0xff);
    header->contentLengthB0  = (unsigned char) ((contentLength     ) & 0xff);
    header->paddingLength    = (unsigned char) paddingLength;
    header->reserved         =  0;
}

void fcgi_begin_request_body_init(int role, int keepConn, /*out*/FCGI_BeginRequestBody* body) {
    body->roleB1 = (unsigned char) ((role >>  8) & 0xff);
    body->roleB0 = (unsigned char) (role         & 0xff);
    body->flags  = (unsigned char) ((keepConn) ? FCGI_KEEP_CONN : 0);
    memset(body->reserved, 0, sizeof(body->reserved));
}

void fcgi_end_request_body_init(int appStatus, int protocolStatus, /*out*/FCGI_EndRequestBody* body) {
    body->appStatusB3    = (unsigned char) ((appStatus >> 24) & 0xff);
    body->appStatusB2    = (unsigned char) ((appStatus >> 16) & 0xff);
    body->appStatusB1    = (unsigned char) ((appStatus >>  8) & 0xff);
    body->appStatusB0    = (unsigned char) ((appStatus      ) & 0xff);
    body->protocolStatus = (unsigned char) protocolStatus;
    memset(body->reserved, 0, sizeof(body->reserved)); 
}

void fcgi_begin_request_record_init(
        int type, int requestId, int contentLength, int paddingLength,
        int role, int keepConn,
        /*out*/FCGI_BeginRequestRecord* record
    ) {
    fcgi_header_init(type, requestId, contentLength, paddingLength, &(record->header));
    fcgi_begin_request_body_init(role, keepConn, &(record->body));
}

void fcgi_end_request_record_init(
        int type, int requestId, int contentLength, int paddingLength,
        int appStatus, int protocalStatus,
        /*out*/FCGI_EndRequestRecord* record
    ) {
    fcgi_header_init(type, requestId, contentLength, paddingLength, &(record->header));
    fcgi_end_request_body_init(appStatus, protocalStatus, &(record->body));
}

int fcgi_get_request_id(FCGI_Header* header) {
    return (header->requestIdB1 << 8) + header->requestIdB0;
}

int fcgi_get_content_length(FCGI_Header* header) {
    return (header->contentLengthB1 << 8) + header->contentLengthB0;
}

int fcgi_get_role(FCGI_BeginRequestBody* body) {
    return (body->roleB1 << 8) + body->roleB0;
}

int fcgi_get_app_status(FCGI_EndRequestBody* body) {
    return (body->appStatusB3 << 24) + (body->appStatusB2 << 16) + (body->appStatusB1 << 8) + body->appStatusB0;
}
