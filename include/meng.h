#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#include <stdbool.h>
#define MENG_API extern "C"
#else
#include <stdbool.h>
#define MENG_API
#endif

#define MENG_VERSION "1.1.0"
#define MENG_VERSION_NUM 110
#define MENG_AUTHOR "esrrhs@163.com"

struct meng;

// 协程入口函数类型
typedef void (*meng_main)(meng * m, void * arg, size_t argsize);

// 创建一个协程
// func: 协程入口函数
// stacksize: 协程栈大小（字节），若为0则使用默认大小（64KB）
// arg: 传递给协程的参数指针（会被拷贝到协程栈区前置内存中）
// argsize: 参数大小（字节）
MENG_API meng * meng_create(meng_main func, size_t stacksize, const void * arg, size_t argsize);

// 执行协程（从起点或上次yield处继续执行，直到yield或执行完毕返回）
MENG_API void meng_run(meng * m);

// 判断协程是否已执行完毕
MENG_API bool meng_end(meng * m);

// 释放并销毁协程
MENG_API void meng_delete(meng * m);

// 主动让出执行权，切换回调用者
MENG_API void meng_yield(meng * m);

// 获取当前正在执行的协程指针（如果在主执行流中则返回NULL）
MENG_API meng * meng_running(void);

// 当前正在执行的协程主动让出执行权
MENG_API void meng_yield_current(void);

// 获取协程绑定的参数地址
MENG_API void * meng_get_arg(meng * m);

// 获取协程绑定的参数大小
MENG_API size_t meng_get_arg_size(meng * m);

// 获取协程栈大小
MENG_API size_t meng_get_stack_size(meng * m);
