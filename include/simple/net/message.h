#ifndef SIMPLE_BUFFER_H
#define SIMPLE_BUFFER_H

#include "define.h"

SimpleMessage* simple_message_create();

void simple_message_destroy(SimpleMessage* self);

#endif
