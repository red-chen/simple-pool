/*
 * 一个简单的内存池，便于统一分配中和销毁内存
 * 
 * 内存池在生成的时候，会预分配一块内存，并将内存拆分成多个页，小内存
 * 的申请，都走Page，大内内存的申请直接走系统，并将获取到的内存挂靠在
 * 内存池上。用户释放内存的时候，内存池回收内存，并将空闲的也挂到空闲
 * 的列表下。
 *
 * 数据申请内存由两部分组成，header和body，header主要存储是内存所在的
 * 位置，长度等，body主要用户用户数据存储
 *
 * 用户获取body的指针地址
 */
#ifndef SIMPLE_MEMORY_POOL_H
#define SIMPLE_MEMORY_POOL_H

typedef struct memory_pool_t MemoryPool;

// 创建内存池
MemoryPool* simple_memory_pool_create(uint32_t size);

// 从内存池中申请内存 
// desc : 描述内存的用途，方便调试
void* simple_memory_pool_malloc(MemoryPool* self, size_t size, char* desc);

// 将申请的内存释放
void simple_memory_pool_free(MemoryPool* self, void* ptr);

// 清空内存池中的数据
void simple_memory_pool_clear(MemoryPool* self);

// 内存指针引用拷贝，注意：传入的指针只能来自于pool中申请的内存
void* simple_memory_ref(void* ptr);

// 销毁内存池对象
void simple_memory_pool_destroy(MemoryPool* self);

// 显示内存池的状态，用于调试
void simple_memory_pool_status(MemoryPool* self);

#endif
