#ifndef SIMPLE_GLOBAL_H
#define SIMPLE_GLOBAL_H

#define SE_OK           0
#define SE_ERR         -1

#define SE_NOMORE      -2  // 表示不再执行
#define SE_AGAIN       -3  // 表示继续执行

#define SE_NONE         0
#define SE_READABLE     1
#define SE_WRITABLE     2

#define SE_FILE_EVENTS  1
#define SE_TIME_EVENTS  2
#define SE_ALL_EVENTS   (SE_FILE_EVENTS|SE_TIME_EVENTS)
#define SE_DONT_WAIT    4

#endif
