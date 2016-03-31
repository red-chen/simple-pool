#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "../event_loop.h"

typedef struct simple_io_handler_t {
    FileFunc* connect; 
    FileFunc* read;
    FileFunc* write;
} SimpleIOHandler;

#endif
