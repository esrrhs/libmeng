#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MENG_VERSION "1.0"
#define MENG_VERSION_NUM 100
#define MENG_AUTHOR "esrrhs@163.com"

#define MENG_API extern "C"

struct meng;

// 入口
typedef void(*meng_main)(meng * m, void * arg, size_t argsize);

// 创建一个协程
MENG_API meng * meng_create(meng_main func, size_t stacksize, void * arg, size_t argsize);

// 执行协程
MENG_API void meng_run(meng * m);

// 是否执行完
MENG_API bool meng_end(meng * m);

// 清除
MENG_API void meng_delete(meng * m);

// 主动切换协程
MENG_API void meng_yield(meng * m);
