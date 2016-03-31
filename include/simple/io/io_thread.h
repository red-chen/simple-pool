#ifndef SIMPLE_IO_THREAD_H_
#define SIMPLE_IO_THREAD_H_

#include "../event_loop.h"

#include <pthread.h>
#include <stdbool.h>

typedef struct simple_io_thread_t SimpleIOThread;

/**
 * 创建创建一个IO线程，在线程内部启动一个event loop
 */
SimpleIOThread* simple_io_thread_create();

void simple_io_thread_destroy(SimpleIOThread* self);

/**
 * 启动线程，并启动loop
 */
void simple_io_thread_start(SimpleIOThread* self);

void simple_io_thread_stop(SimpleIOThread* self);

void simple_io_thread_join(SimpleIOThread* self);

EventLoop* simple_io_thread_get_loop(SimpleIOThread* self);

/**
 * 向线程中添加文件事件
 */
int simple_io_thread_add_file_event(
        SimpleIOThread* self,
        int fd,
        int mask,
        FileFunc* func,
        void* user_data);

/**
 * 删除文件事件
 */
void simple_io_thread_del_file_event(
	SimpleIOThread* self, 
	int fd, 
	int mask);

int64_t simple_io_thread_add_time_event(
        SimpleIOThread* self,
        int64_t millisecond,
        TimeFunc* func,
        void* user_data,
        FinalizeFunc* final);

void simple_io_thread_del_time_event(
	SimpleIOThread* self, 
	int64_t timeEventId);

#endif /* SIMPLE_IO_THREAD_H_ */
