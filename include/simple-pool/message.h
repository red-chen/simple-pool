/**
 * Message 就是一个连续的内存块，行为和一个FIFO的队列一致
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
void* simple_message_get_push_ptr(SimpleMessage* self, int size);

/**
 * 将内存游标向后移动size位
 *
 * 返回值
 *     0表示成功
 *     负数表示不够移动，还缺的位数
 */
int simple_message_set_push_size(SimpleMessage* self, int size);

/**
 * @brief 获取可以读取数据的起始指针
 *
 * @param self
 *
 * @return 
 */
void* simple_message_get_pull_ptr(SimpleMessage* self);

/**
 * @brief 设置已经读取的数据量
 *
 * @param self
 * @param size
 *
 * @return 
 */
int simple_message_set_pull_size(SimpleMessage* self, int size);

/**
 * 获取数据的大小
 */
int simple_message_size(SimpleMessage* self);

/**
 * 清除所有数据
 */
void simple_message_clear(SimpleMessage* self);

int simple_message_add(SimpleMessage* self, void* data, int size);

void* simple_message_get(SimpleMessage* self);

int simple_message_get_free_size(SimpleMessage* self);
#endif
