// 主要是提供常用的宏定义
#ifndef SIMPLE_ASSERT_H_
#define SIMPLE_ASSERT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <execinfo.h>

void va_list_print(char log_message[], char* format, ...);

#define ASSERT_NOT_NULL(flag) \
        if(!((flag) != NULL)){\
            char __buffer_stack__[1024];\
            void *__array__[10];\
            char **__strframe__ = NULL;\
            int __i__, __idx__=0, __n__ = backtrace(__array__, 10);\
            __strframe__ = (char **)backtrace_symbols(__array__, __n__);\
            for (__i__ = 0; __i__ < __n__; __i__++) \
            __idx__ += snprintf(__idx__+__buffer_stack__, 256, "%s\n", __strframe__[__i__]);\
            free(__strframe__);\
            fprintf(stderr, "Assert fail ! pointer is NULL.\t[%s:%d]\n%s\n", __FILE__, __LINE__, __buffer_stack__);\
            abort();\
        }

#define ASSERT(flag, ...)\
        if(!(flag)){\
            char __buffer_stack__[1024];\
            void *__array__[10];\
            char **__strframe__ = NULL;\
            int __i__, __idx__=0, __n__ = backtrace(__array__, 10);\
            __strframe__ = (char **)backtrace_symbols(__array__, __n__);\
            for (__i__ = 0; __i__ < __n__; __i__++) \
            __idx__ += snprintf(__idx__+__buffer_stack__, 256, "%s\n", __strframe__[__i__]);\
            free(__strframe__);\
            char __log_message__[512] = { 0 };\
            va_list_print(__log_message__, __VA_ARGS__);\
            fprintf(stderr, "Assert fail ! %s\t[%s:%d]\n%s\n", (__log_message__),  __FILE__, __LINE__, __buffer_stack__);\
            abort();\
        }

#endif /* ASSERT_H_ */
