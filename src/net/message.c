#include "net/message.h"

#include "assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_MEM_SIZE 65536  // 64K

struct simple_message_t {
    int capacity;
    int size;
    void* push_index;
    void* pull_index;
    char data[MESSAGE_MEM_SIZE];
};

SimpleMessage* simple_message_create() {
    SimpleMessage* self = malloc(sizeof(SimpleMessage));
    self->capacity = MESSAGE_MEM_SIZE;
    self->size = 0;
    self->push_index = self->data;
    self->pull_index = self->data;
    return self;
}

void* simple_message_get_push_ptr(SimpleMessage* self, int size) {
    int f = self->capacity - self->size;
    if (size > f) {
        WARN("alloc mem fail. size: %d.", size);
        return NULL;
    }
    return self->push_index;
}

int simple_message_set_push_size(SimpleMessage* self, int size) {
    int f = self->capacity - self->size;
    if (size > f) {
        WARN("push data fail. size: %d.", size);
        return f - size;
    }
    self->push_index += size;
    self->size += size;
    return 0;
}

void* simple_message_get_pull_ptr(SimpleMessage* self) {
    return self->pull_index;
}

int simple_message_set_pull_size(SimpleMessage* self, int size) {
    if (size > self->size) {
        WARN("set pull size fail. size: %d.", size);
        return self->size - size;
    }
    self->pull_index += size;
    self->size -= size;

    // NOTE 优化，当空闲的空间占据二分之一且push index已经达到末尾，将
    // 数据移动到头部
    if ((self->push_index == self->data + self->capacity) && 
            (self->size < self->capacity / 2)) {
        memcpy(self->data, self->pull_index, self->size);
        self->pull_index = self->data;
        self->push_index = self->data + self->size;
    }
    return 0;
}

int simple_message_size(SimpleMessage* self) {
    return self->size;
}

void simple_message_destroy(SimpleMessage* self) {
    free(self);
}

void simple_message_clear(SimpleMessage* self) {
    self->push_index = self->data;
    self->pull_index = self->data;
    self->size = 0;

    bzero(self->data, self->capacity);
}

int simple_message_add(SimpleMessage* self, void* data, int size) {
    int f = self->capacity - self->size;
    if (size > f) {
        WARN("add data fail. size: %d.", size);
        return f - size;
    }
    memcpy(self->push_index, data, size);
    self->push_index += size;
    self->size += size;
    return 0;
}

void* simple_message_get(SimpleMessage* self) {
    return self->pull_index;
}

int simple_message_get_free_size(SimpleMessage* self) {
    return (void*)(self->data + self->capacity) - self->push_index;
}
