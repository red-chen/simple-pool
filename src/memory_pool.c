#include "memory_pool.h"

typedef struct memory_header_t {
#ifdef DEBUG
    char magic[8];
    char desc[8];
#endif
    int len;
    int hit;
    int ref; // 被引用的次数
} MemoryHeader;

typedef struct memory_page_t {
    MemoryHeader header;
    void* body;
} MemoryPage;

struct memory_pool_t {
    int page_size;
    int totle_count;
    int free_count;
    void* pool;
}
