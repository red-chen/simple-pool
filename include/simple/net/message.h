/**
 * Message 就是一个连续的内存块
 *
 * TODO 为了实现简单，就暴力申请64K堆上的内存，不够再重新分配
 */
#ifndef SIMPLE_BUFFER_H
#define SIMPLE_BUFFER_H

#include "define.h"

SimpleMessage* simple_message_create();

void simple_message_destroy(SimpleMessage* self);

/**
 * 尝试获取指定大小的内存
 */
void* simple_message_pointer(SimpleMessage* self, int size);

/**
 * 将内存游标向后移动size位
 *
 * 返回值
 *     0表示成功
 *     负数表示不够移动，还缺的位数
 */
int simple_message_move(SimpleMessage* self, int size);

/**
 * 获取数据的大小
 */
int simple_message_size(SimpleMessage* self);

/**
 * 获取数据
 */
void* simple_message_data(SimpleMessage* self);

/**
 * 清除所有数据
 */
void simple_message_clear(SimpleMessage* self);
#endif
