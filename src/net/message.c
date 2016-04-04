#include "net/message.h"

#include "assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_MEM_SIZE 65536  // 64K

struct simple_message_t {
    int capacity;
    int size;
    void* index;
    char data[MESSAGE_MEM_SIZE];
};

SimpleMessage* simple_message_create() {
    SimpleMessage* self = malloc(sizeof(SimpleMessage));
    self->capacity = MESSAGE_MEM_SIZE;
    self->size = 0;
    self->index = self->data;
    return self;
}

void* simple_message_pointer(SimpleMessage* self, int size) {
    // TODO refine  这里太过于简单了
    int f = self->capacity - self->size;
    if (size > f) {
        ASSERT(false, "alloc mem fail. size: %d.", size);
    }
    return self->index;
}

int simple_message_move(SimpleMessage* self, int size) {
    int f = self->capacity - self->size;
    // TODO 这里考虑快速开发，直接检查失败退出即可，后面需要refine
    if (size > f) {
        ASSERT(false, "move pointer fail. size: %d.", size);
    }
    self->index += size;
    self->size += size;
    return 0;
}

int simple_message_size(SimpleMessage* self) {
    return self->size;
}

void* simple_message_data(SimpleMessage* self) {
    return self->data;
}

void simple_message_destroy(SimpleMessage* self) {
    free(self);
}

void simple_message_clear(SimpleMessage* self) {
    self->index = self->data;
    self->size = 0;

    bzero(self->index, self->capacity);
}
