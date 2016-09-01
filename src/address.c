#include "net/address.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct simple_address_t {
    int family; // AF_INET, AF_INET6, AF_UNIX
    char addr[32];
    union {
        int port;
        char* unix_path;
    };
};

SimpleAddress* simple_address_create_inet(const char* ip, int port) {
    SimpleAddress* self = malloc(sizeof(SimpleAddress));
    self->family = AF_INET;
    if (ip != NULL) {
        strcpy(self->addr, ip);
    }
    self->port = port;
    return self;
}

SimpleAddress* simple_address_create_inet6(const char* ip, int port) {
    SimpleAddress* self = malloc(sizeof(SimpleAddress));
    self->family = AF_INET6;
    if (ip != NULL) {
        strcpy(self->addr, ip);
    }
    self->port = port;
    return self;
}

SimpleAddress* simple_address_create_unix(const char* unix_path) {
    SimpleAddress* self = malloc(sizeof(SimpleAddress));
    self->family = AF_UNIX;
    self->unix_path = strdup(unix_path);
    return self;
}

int simple_address_get_family(SimpleAddress* self) {
    return self->family;
}

const char* simple_address_get_addr(SimpleAddress* self) {
    return self->addr;
}

int simple_address_get_port(SimpleAddress* self) {
    return self->port;
}

const char* simple_address_get_unix_path(SimpleAddress* self) {
    return self->unix_path;
}

void simple_address_destroy(SimpleAddress* self) {
    if (self->family == AF_UNIX) {
        free(self->unix_path);
    }
    free(self);
}
