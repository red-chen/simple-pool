#ifndef SIMPLE_COUNTER_H_
#define SIMPLE_COUNTER_H_

#include <stdint.h>

typedef struct counter_t {
    int64_t raw; 
    int64_t history_row;
    int64_t base;
    int64_t history_base;
    int64_t history_timestamp;
} Counter;

#endif /* COUNTER_H_ */
